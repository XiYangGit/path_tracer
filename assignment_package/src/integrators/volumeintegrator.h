#pragma once
#include "integrator.h"
#include "scene/lights/visibiliytester.h"
class VolumeIntegrator:public Integrator
{
public:
    VolumeIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : Integrator(bounds, s, sampler, recursionLimit)
    {}

    // Evaluate the energy transmitted along the ray back to
    // its origin using multiple importance sampling
    virtual Color3f Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f beta) const;
    Color3f UniformSampleOneLight(const Intersection &it,const Scene &scene, std::shared_ptr<Sampler> sampler,bool handleMedia=false) const;
    Color3f EstimateDirect(const Intersection &it,
    const Point2f &uScattering, const std::shared_ptr<Light> light,
    const Point2f &uLight,const Scene &scene, std::shared_ptr<Sampler> sampler,bool handleMedia=false, bool specular=false) const;
    Color3f Tr(const Intersection &it,const Intersection &lightsample,const Scene &scene, std::shared_ptr<Sampler> sampler) const ;
private:
    const Float rrThreshold=1.0f;
};

Float MaxComponentValue(const Color3f &c);

