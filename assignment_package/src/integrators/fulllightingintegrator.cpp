#include "fulllightingintegrator.h"
//Instruction:
/*This function is for full lighting integrator, one of the most important features in path tracer system.
  It simulates photon path and recursively computes the final illumination at certain point.
  Use multiple importance sampling method, and combine sampling result from both materials and light sources with weights.

*/

Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth,Color3f beta1) const
{
    Color3f L(0.f), beta(1.f);
    Ray currentRay(ray);
    bool specularBounce = false;
    int bounces=recursionLimit-depth;
    while(bounces<recursionLimit) {
        //int bounces=recursionLimit-depth;
        //Intersect ray with scene
        Intersection isect;
        Vector3f wo = -currentRay.direction;
        bool foundIntersection = scene.Intersect(currentRay,&isect);
        //Possibly add emitted light at intersection
        if (bounces == 0 || specularBounce) {
        //Add emitted light
            if (foundIntersection)
            L += beta * isect.Le(wo);
        }
        //Terminate path if ray escaped or maxDepth was reached
        if (!foundIntersection || bounces >= recursionLimit)
        break;
        if(isect.objectHit->areaLight)
            break;
        //Compute scattering functions and skip over medium boundaries
        if(!isect.objectHit->GetMaterial())
                break;
        if (!isect.bsdf) {
        currentRay = isect.SpawnRay(currentRay.direction);
        bounces--;
        continue;
        }
        //Sample illumination from lights to find path contribution
        Point2f uLight = sampler->Get2D();
            Point2f uScattering = sampler->Get2D();
            int nLights = int(scene.lights.size());

            int lightNum = std::min((int)(sampler->Get1D()*nLights),nLights-1);
            const std::shared_ptr<Light> &light = scene.lights[lightNum];
            Color3f Ld(0.f);
            if(nLights>0){
                BxDFType bsdfFlags = specularBounce ? BSDF_ALL :
                BxDFType(BSDF_ALL & ~BSDF_SPECULAR);
        //Sample light source with multiple importance sampling
            Vector3f wiL;
            Float lightPdf = 0, scatteringPdf = 0;
            Color3f Li = light->Sample_Li(isect, uLight, &wiL, &lightPdf);
            //lightPdf/=nLights;
            wiL=glm::normalize(wiL);
            if (lightPdf > 0.0f && !IsBlack(Li)) {
            //Compute BSDF  for light sample
                Color3f f(0.0f);
                f=isect.bsdf->f(wo,wiL,bsdfFlags);
                scatteringPdf=isect.bsdf->Pdf(wo,wiL,bsdfFlags);
            if (!IsBlack(f)) {
            //Compute visibility
                Ray backRay=isect.SpawnRay(wiL);
                Intersection backIsect;
                bool back_inter=scene.Intersect(backRay,&backIsect);
                if(back_inter)
                 {
                    Point3f lightPos(0.0f);
                    if(light->getPosition(&lightPos))  //test visibility of point light
                    {
                        if(glm::distance(backIsect.point,isect.point)<glm::distance(lightPos,isect.point))
                        Li=Color3f(0.0f);
                    }else{
                const std::shared_ptr<Light> &b=backIsect.objectHit->areaLight;
                if(!( light.get()==b.get()))
                Li=Color3f(0.0f);}
                }
            //Add light’s contribution to reflected radiance
                if (!IsBlack(Li)) {
                Float weight = PowerHeuristic(1, lightPdf, 1, scatteringPdf);
                Ld += f * Li * weight*AbsDot(isect.normalGeometric,wiL) / lightPdf;
                }
            }
            }

            //Sample BSDF with multiple importance sampling

            Color3f f(0.0f);

            //Sample scattered direction
            BxDFType sampledType;
            bool sampledSpecular = false;
            Vector3f wiS;
            Float lightPdf2 = 0, scatteringPdf2 = 0;
            f = isect.bsdf->Sample_f(wo, &wiS, uScattering, &scatteringPdf2,
            bsdfFlags, &sampledType);
            wiS=glm::normalize(wiS);
            //f *= AbsDot(wiS, isect.normalGeometric);
            sampledSpecular =(sampledType&BSDF_SPECULAR) != 0;
            if(!sampledSpecular){
            if (!IsBlack(f) && scatteringPdf2 > 0.0f) {
            //Account for light contributions along sampled direction wi
                Float weight = 1;
                if (!sampledSpecular) {
                lightPdf2 = light->Pdf_Li(isect, wiS);
                //lightPdf2/=nLights;
                weight = PowerHeuristic(1, scatteringPdf2, 1, lightPdf2);
                if (fabs(lightPdf2 )<FLT_EPSILON)
                    weight=0.0f;
                }
                //Add light contribution from material sampling
                Color3f Li(0.0f);

                Ray backRay2=isect.SpawnRay(wiS);
                Intersection backIsect2;
                bool back_inter2=scene.Intersect(backRay2,&backIsect2);
                if(back_inter2)
                {
                    Point3f lightPos(0.0f);
                    if(light->getPosition(&lightPos))  //test visibility of point light
                    {
                        if(glm::distance(backIsect2.point,isect.point)<glm::distance(lightPos,isect.point))
                        Li=Color3f(0.0f);
                        else
                          Li=isect.Le(-wiS);
                    }else{
                    const std::shared_ptr<Light> &b2=backIsect2.objectHit->areaLight;
                    if(!(light.get()==b2.get()))
                    Li=Color3f(0.0f);
                    else
                    Li=isect.Le(-wiS);}
                }

                if (!IsBlack(Li))
                    Ld=Color3f(0.0f);
                Ld += f * Li *  weight*AbsDot(isect.normalGeometric,wiS) / scatteringPdf2;
            }
            }}
            L+=beta*Ld*(float)nLights;;

        //Sample BSDF to get new path direction
        Vector3f  wi;
        Float pdf;
        BxDFType flags;
        Color3f f = isect.bsdf->Sample_f(wo, &wi, sampler->Get2D(),
        &pdf, BSDF_ALL, &flags);
        if (IsBlack(f) || fabs(pdf)<FLT_EPSILON)
        break;
        beta *= f * AbsDot(wi, isect.normalGeometric) / pdf;
        specularBounce = (flags & BSDF_SPECULAR) != 0;
        currentRay = isect.SpawnRay(wi);

        //Possibly terminate the path with Russian roulette
        if (bounces > 3) {
        Float q = std::max((Float).05, 1 - beta.y);
        if (sampler->Get1D() < q)
        break;
        beta /= 1 - q;
        }
        bounces++;


    }
    return L;




}


