#include "volumeintegrator.h"
#include <iostream>
Color3f VolumeIntegrator::Li(const Ray &r, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f beta1) const
{
    Color3f L(0.0f), beta(1.0f);
        //RayDifferential ray(r);
    Ray ray(r);
        bool specularBounce = false;
        int bounces;
        // Added after book publication: etaScale tracks the accumulated effect
        // of radiance scaling due to rays passing through refractive
        // boundaries (see the derivation on p. 527 of the third edition). We
        // track this value in order to remove it from beta when we apply
        // Russian roulette; this is worthwhile, since it lets us sometimes
        // avoid terminating refracted rays that are about to be refracted back
        // out of a medium and thus have their beta value increased.
        Float etaScale = 1;

        for (bounces = 0;; ++bounces) {
            //std::cout<<"bounces: "<<bounces<<std::endl;
            // Intersect _ray_ with scene and store intersection in _isect_
            Intersection isect;
            bool foundIntersection = scene.Intersect(ray, &isect);
            isect.wo=-ray.direction;
            if(!foundIntersection) break;
            // Sample the participating medium, if present
            Intersection mi;  //medium intersection
            if (ray.medium) beta *= ray.medium->Sample(ray, sampler, &mi);
            //mi.wo=-ray.direction;
            if (IsBlack(beta)) break;

            // Handle an interaction with a medium or a surface
            if (mi.IsValid()) {
                // Terminate path if ray escaped or _maxDepth_ was reached
                if (bounces >= recursionLimit) break;

                //++volumeInteractions;
                // Handle scattering at point in medium for volumetric path tracer
//                const Distribution1D *lightDistrib =
//                    lightDistribution->Lookup(mi.p);
                L += beta * UniformSampleOneLight(mi, scene, sampler, true);

                Vector3f wo = -ray.direction, wi;
                mi.phase->Sample_p(wo, &wi, sampler->Get2D());
                ray = mi.SpawnRay(wi);
            } else {
//                //++surfaceInteractions;
//                // Handle scattering at point on surface for volumetric path tracer

//                // Possibly add emitted light at intersection
//                if(isect.objectHit->areaLight!=nullptr &&bounces>0 )
//                            break;
//                if(!isect.objectHit->GetMaterial())
//                                break;
//                if (bounces == 0 || specularBounce) {
//                    // Add emitted light at path vertex or from the environment
//                    if (foundIntersection)
//                        L += beta * isect.Le(-ray.direction);
//                    else
//                        L=Color3f(0.0f);
////                        for (const auto &light : scene.infiniteLights)
////                            L += beta * light->Le(ray);
//                }

//                // Terminate path if ray escaped or _maxDepth_ was reached
//                if (!foundIntersection || bounces >= recursionLimit) break;


//                // Compute scattering functions and skip over medium boundaries
//                //isect.ComputeScatteringFunctions(ray, arena, true);
//                if (!isect.bsdf) {
//                    ray = isect.SpawnRay(ray.direction);
//                    bounces--;
//                    continue;
//                }

//                // Sample illumination from lights to find attenuated path
//                // contribution
////                const Distribution1D *lightDistrib =
////                    lightDistribution->Lookup(isect.p);
//                L += beta * UniformSampleOneLight(isect, scene, sampler,
//                                                  true);

//                // Sample BSDF to get new path direction
//                Vector3f wo = -ray.direction, wi;
//                Float pdf;
//                BxDFType flags;
//                Color3f f = isect.bsdf->Sample_f(wo, &wi, sampler->Get2D(), &pdf,
//                                                  BSDF_ALL, &flags);
//                if (IsBlack(f) || pdf == 0.f) break;
//                beta *= f * AbsDot(wi, isect.normalGeometric) / pdf;
//                //DCHECK(std::isinf(beta.y()) == false);
//                specularBounce = (flags & BSDF_SPECULAR) != 0;
//                if ((flags & BSDF_SPECULAR) && (flags & BSDF_TRANSMISSION)) {
//                    Float eta = isect.bsdf->eta;
//                    // Update the term that tracks radiance scaling for refraction
//                    // depending on whether the ray is entering or leaving the
//                    // medium.
//                    etaScale *=
//                        (glm::dot(wo, isect.normalGeometric) > 0) ? (eta * eta) : 1 / (eta * eta);
//                }
//                ray = isect.SpawnRay(wi);



                        Vector3f wo=-ray.direction;
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
                        ray = isect.SpawnRay(ray.direction);
                        bounces--;
                        continue;
                        }

                            isect.wo=wo;
                            L+=beta*UniformSampleOneLight(isect, scene, sampler, false);

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
                                        if ((flags & BSDF_SPECULAR) && (flags & BSDF_TRANSMISSION)) {
                                            Float eta = isect.bsdf->eta;
                                            // Update the term that tracks radiance scaling for refraction
                                            // depending on whether the ray is entering or leaving the
                                            // medium.
                                            etaScale *=
                                                (glm::dot(wo, isect.normalGeometric) > 0) ? (eta * eta) : 1 / (eta * eta);
                                        }
                        ray = isect.SpawnRay(wi);
            }

            // Possibly terminate the path with Russian roulette
            // Factor out radiance scaling due to refraction in rrBeta.
            Color3f rrBeta = beta * etaScale;
            if (MaxComponentValue(rrBeta) < rrThreshold && bounces > 3) {
                Float q = std::max((Float).05, 1 - MaxComponentValue(rrBeta));
                if (sampler->Get1D() < q) break;
                beta /= 1 - q;
                //DCHECK(std::isinf(beta.y) == false);
            }
        }
        //ReportValue(pathLength, bounces);
        return L;
}
Color3f VolumeIntegrator::UniformSampleOneLight(const Intersection &it,const Scene &scene, std::shared_ptr<Sampler> sampler, bool handleMedia) const
{

        // Randomly choose a single light to sample, _light_
        //int nLights = int(scene.lights.size());
        int nLights=int(scene.lights.size());
        if (nLights == 0) return Color3f(0.f);
        int lightNum;
        Float lightPdf;

        lightNum = std::min((int)(sampler->Get1D() * nLights), nLights - 1);
        lightPdf = Float(1) / nLights;

        const std::shared_ptr<Light> &light = scene.lights[lightNum];
        Point2f uLight = sampler->Get2D();
        Point2f uScattering = sampler->Get2D();
        Color3f result= EstimateDirect(it, uScattering, light, uLight,scene,sampler,
                               handleMedia) / lightPdf;
        //std::cout<<"estimate direct: "<<result[0]<<", "<<result[1]<<", "<<result[2]<<std::endl;
        return result;
}

Color3f VolumeIntegrator::EstimateDirect(const Intersection &it, const Point2f &uScattering, const std::shared_ptr<Light> light, const Point2f &uLight,const Scene &scene, std::shared_ptr<Sampler> sampler, bool handleMedia, bool specular) const
{
//return Color3f(0.0f);
    BxDFType bsdfFlags =
            specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);
        Color3f Ld(0.f);
        // Sample light source with multiple importance sampling
        Vector3f wi;
        Float lightPdf = 0, scatteringPdf = 0;
        Intersection LightSample;
        Color3f Li = light->Sample_Li(it, uLight, &wi, &lightPdf,&LightSample);
        if (lightPdf > 0 && !IsBlack(Li)) {
            // Compute BSDF or phase function's value for light sample
            Color3f f;
            if (it.IsSurfaceInteraction()) {
                // Evaluate BSDF for light sampling strategy
                const Intersection &isect = (const Intersection &)it;
                f = isect.bsdf->f(isect.wo, wi, bsdfFlags) *
                    AbsDot(wi, isect.normalGeometric);
                scatteringPdf = isect.bsdf->Pdf(isect.wo, wi, bsdfFlags);

            } else {
                // Evaluate phase function for light sampling strategy
                const Intersection &mi = (const Intersection &)it;
                Float p = mi.phase->p(mi.wo, wi);
                f = Color3f(p);
                scatteringPdf = p;

            }
            if (!IsBlack(f)) {
                // Compute effect of visibility for light source sample
                if (handleMedia) {
                    Li *= Tr(it,LightSample,scene, sampler);
                    //VLOG(2) << "  after Tr, Li: " << Li;
                } else {
                    //old method for visibility
                    Ray backRay=it.SpawnRay(wi);
                    Intersection backIsect;
                    bool back_inter=scene.Intersect(backRay,&backIsect);
                    if(back_inter)
                     {
                        Point3f lightPos(0.0f);
                        if(light->getPosition(&lightPos))  //test visibility of point light
                        {
                            if(glm::distance(backIsect.point,it.point)<glm::distance(lightPos,it.point))
                            Li=Color3f(0.0f);
                        }else{
                         if(backIsect.objectHit->areaLight){

                    const std::shared_ptr<Light> &b=backIsect.objectHit->areaLight;
                    if(!( light.get()==b.get()))
                    Li=Color3f(0.0f);}}
                    }
                }

                // Add light's contribution to reflected radiance
                if (!IsBlack(Li)) {

                        Float weight =
                            PowerHeuristic(1, lightPdf, 1, scatteringPdf);
                        Ld += f * Li * weight / lightPdf;

                }
            }
        }

        // Sample BSDF with multiple importance sampling

            Color3f f(0.0f);
            bool sampledSpecular = false;
            if (it.IsSurfaceInteraction()) {
                //if (it.IsSurfaceIntersection) {
                // Sample scattered direction for surface interactions
                BxDFType sampledType;
                const Intersection &isect = (const Intersection &)it;
                f = isect.bsdf->Sample_f(isect.wo, &wi, uScattering, &scatteringPdf,
                                         bsdfFlags, &sampledType);
                f *= AbsDot(wi, isect.normalGeometric);
                sampledSpecular = (sampledType & BSDF_SPECULAR) != 0;
            } else {
                // Sample scattered direction for medium interactions
                const Intersection &mi = (const Intersection &)it;
                Float p = mi.phase->Sample_p(mi.wo, &wi, uScattering);
                f = Color3f(p);
                scatteringPdf = p;
                //std::cout<<"p: "<<p<<std::endl;
            }
            if (!IsBlack(f) && scatteringPdf > 0) {
                // Account for light contributions along sampled direction _wi_
                Float weight = 1;
                if (!sampledSpecular) {
                    lightPdf = light->Pdf_Li(it, wi);
                    if (lightPdf == 0) return Ld;
                    weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
                }

                // Find intersection and compute transmittance
                Intersection lightIsect;
                Ray ray = it.SpawnRay(wi);
                Color3f Tr(1.f);
                bool foundSurfaceInteraction =
                    handleMedia ? scene.IntersectTr(ray, sampler, &lightIsect, &Tr)
                                : scene.Intersect(ray, &lightIsect);

                // Add light contribution from material sampling
                Color3f Li(0.f);
                if (foundSurfaceInteraction) {
                    if (lightIsect.objectHit->GetAreaLight() == light.get())
                        Li = lightIsect.Le(-wi);
//                    Ray backRay2=lightIsect.SpawnRay(wi);
//                    Intersection backIsect2;
//                    bool back_inter2=scene.Intersect(backRay2,&backIsect2);
//                    if(back_inter2)
//                    {
//                        Point3f lightPos(0.0f);
//                        if(light->getPosition(&lightPos))  //test visibility of point light
//                        {
//                            if(glm::distance(backIsect2.point,lightIsect.point)<glm::distance(lightPos,lightIsect.point))
//                            Li=Color3f(0.0f);
//                            else
//                              Li=lightIsect.Le(-wi);
//                        }else{
//                            if(backIsect2.objectHit->areaLight){

//                        const std::shared_ptr<Light> &b2=backIsect2.objectHit->areaLight;
//                        if(!(light.get()==b2.get()))
//                        Li=Color3f(0.0f);
//                        else
//                        Li=lightIsect.Le(-wi);}}
//                    }
                } /*else
                    Li = light->Le(ray);*/
                if (!IsBlack(Li)) Ld += f * Li * Tr * weight / scatteringPdf;
            }

        return Ld;
}

//bool VisibilityTester::Unoccluded(const Scene &scene) const {
//    return !scene.IntersectP(p0.SpawnRayTo(p1));
//}

Color3f VolumeIntegrator::Tr(const Intersection &p0, const Intersection &p1, const Scene &scene, std::shared_ptr<Sampler> sampler) const
{
    Ray ray(p0.SpawnRayTo(p1));
    Color3f Tr(1.f);
    //int iter=0;
    while (true) {
        //iter++;
        Intersection isect;
        bool hitSurface = scene.Intersect(ray, &isect);
        // Handle opaque surface along ray's path
        if (hitSurface && isect.objectHit->GetMaterial() != nullptr)
            return Color3f(0.0f);

        // Update transmittance for current ray segment
        if (ray.medium) Tr *= ray.medium->Tr(ray, sampler);

        // Generate next ray segment or return final transmittance
        if (!hitSurface) break;
        ray = isect.SpawnRayTo(p1);
        //if(iter>10000) return Color3f(1.0f);
    }
    return Tr;
}
Float MaxComponentValue(const Color3f &c)  {
    Float m = c[0];
    for (int i = 1; i < 3; ++i)
        m = std::max(m, c[i]);
    return m;
}


//Color3f VolumeIntegrator::Li(const Ray &r, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f beta1) const
//{
//    Color3f L(0.f), beta(1.f);
//    Ray currentRay(r);
//    bool specularBounce = false;
//    int bounces=recursionLimit-depth;
//    while(bounces<recursionLimit) {
//        //int bounces=recursionLimit-depth;
//        //Intersect ray with scene
//        Intersection isect;
//        Vector3f wo = -currentRay.direction;
//        bool foundIntersection = scene.Intersect(currentRay,&isect);
//        //Possibly add emitted light at intersection
//        if (bounces == 0 || specularBounce) {
//        //Add emitted light
//            if (foundIntersection)
//            L += beta * isect.Le(wo);
//        }
//        //Terminate path if ray escaped or maxDepth was reached
//        if (!foundIntersection || bounces >= recursionLimit)
//        break;
//        if(isect.objectHit->areaLight)
//            break;
//        //Compute scattering functions and skip over medium boundaries
//        if(!isect.objectHit->GetMaterial())
//                break;
//        if (!isect.bsdf) {
//        currentRay = isect.SpawnRay(currentRay.direction);
//        bounces--;
//        continue;
//        }

//            isect.wo=wo;
//            L+=beta*UniformSampleOneLight(isect, scene, sampler, false);

//        //Sample BSDF to get new path direction
//        Vector3f  wi;
//        Float pdf;
//        BxDFType flags;
//        Color3f f = isect.bsdf->Sample_f(wo, &wi, sampler->Get2D(),
//        &pdf, BSDF_ALL, &flags);
//        if (IsBlack(f) || fabs(pdf)<FLT_EPSILON)
//        break;
//        beta *= f * AbsDot(wi, isect.normalGeometric) / pdf;
//        specularBounce = (flags & BSDF_SPECULAR) != 0;
//        currentRay = isect.SpawnRay(wi);

//        //Possibly terminate the path with Russian roulette
//        if (bounces > 3) {
//        Float q = std::max((Float).05, 1 - beta.y);
//        if (sampler->Get1D() < q)
//        break;
//        beta /= 1 - q;
//        }
//        bounces++;


//    }
//    return L;
//}

