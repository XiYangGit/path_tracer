#include "homogeneousmedium.h"
const float MaxFloat=INFINITY;
Color3f HomogeneousMedium::Tr(const Ray &ray, std::shared_ptr<Sampler> sampler) const {
    //ProfilePhase _(Prof::MediumTr);

    //return Exp(-sigma_t * std::min(ray.tMax * glm::length(ray.direction), MaxFloat));
    Color3f ret=-sigma_t * std::min(ray.tMax * glm::length(ray.direction), MaxFloat);
    for (int i = 0; i < 3; ++i) ret[i] = std::exp(ret[i]);
    //return Exp(-sigma_t * std::min(ray.tMax * glm::length(ray.direction), MaxFloat));
    assert(!std::isnan(ret.x) || std::isnan(ret.y) || std::isnan(ret.z));
    return ret;
}
Color3f HomogeneousMedium::Sample(const Ray &ray, std::shared_ptr<Sampler> sampler, Intersection *mi) const
{
    //ProfilePhase _(Prof::MediumSample);
    // Sample a channel and distance along the ray
//    int channel = std::min((int)(sampler.Get1D() * Spectrum::nSamples),
//                           Spectrum::nSamples - 1);
    int channel = std::min((int)(sampler->Get1D() * 3),
                               2);
    Float dist = -std::log(1 - sampler->Get1D()) / sigma_t[channel];
    Float t = std::min(dist * glm::length(ray.direction), ray.tMax);
    bool sampledMedium = t < ray.tMax;
    if (sampledMedium)
       { /**mi = MediumInteraction(ray(t), -ray.direction, ray.time, this,
                                ARENA_ALLOC(arena, HenyeyGreenstein)(g));*/
        //PhaseFunction *newphase=new HenyeyGreenstein(g);
        Intersection isect=Intersection(this,std::make_shared<HenyeyGreenstein>(g));
        isect.point=ray.origin+t*ray.direction;
        isect.wo=-ray.direction;
        *mi=isect;
    }

    // Compute the transmittance and sampling density
    //Color3f Tr = Exp(-sigma_t * std::min(t, MaxFloat) * glm::length(ray.direction));
    Color3f Tr = -sigma_t * std::min(t, MaxFloat) * glm::length(ray.direction);
    for (int i = 0; i < 3; ++i) Tr[i] = std::exp(Tr[i]);
    // Return weighting factor for scattering from homogeneous medium
    Color3f density = sampledMedium ? (sigma_t * Tr) : Tr;
    Float pdf = 0;
    for (int i = 0; i < 3; ++i) pdf += density[i];
    pdf *= 1 / (Float)3;
    if (pdf == 0) {
        //CHECK(Tr.IsBlack());
        if(IsBlack(Tr)) return Color3f(1.0f);

    }
    return sampledMedium ? (Tr * sigma_s / pdf) : (Tr / pdf);
}
