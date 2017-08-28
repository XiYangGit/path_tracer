#pragma once
#include "globals.h"
#include "raytracing/intersection.h"
#include "samplers/sampler.h"
#include <iostream>
class Ray;
class Intersection;
struct MediumInterface;
class PhaseFunction {
  public:
    // PhaseFunction Interface
    virtual ~PhaseFunction();
    virtual Float p(const Vector3f &wo, const Vector3f &wi) const = 0;
    virtual Float Sample_p(const Vector3f &wo, Vector3f *wi,
                           const Point2f &u) const = 0;
    virtual std::string ToString() const = 0;
};

inline std::ostream &operator<<(std::ostream &os, const PhaseFunction &p) {
    os << p.ToString();
    return os;
}

bool GetMediumScatteringProperties(const std::string &name, Color3f *sigma_a,
                                   Color3f *sigma_s);

// Media Inline Functions
inline Float PhaseHG(Float cosTheta, Float g) {
    Float denom = 1 + g * g + 2 * g * cosTheta;
    return Inv4Pi * (1 - g * g) / (denom * std::sqrt(denom));
}

// Medium Declarations
class Medium {
  public:
    // Medium Interface
    virtual ~Medium() {}
    virtual Color3f Tr(const Ray &ray, std::shared_ptr<Sampler> sampler) const = 0;
    virtual Color3f Sample(const Ray &ray, std::shared_ptr<Sampler> sampler,
                           //Sampler &sampler,
                            //MemoryArena &arena,MediumInteraction *mi
                            Intersection *mi) const = 0;
};

// HenyeyGreenstein Declarations
class HenyeyGreenstein : public PhaseFunction {
  public:
    // HenyeyGreenstein Public Methods
    HenyeyGreenstein(Float g) : g(g) {}
    Float p(const Vector3f &wo, const Vector3f &wi) const;
    Float Sample_p(const Vector3f &wo, Vector3f *wi,
                   const Point2f &sample) const;
    std::string ToString() const {
//        return StringPrintf("[ HenyeyGreenstein g: %f ]", g);
        return "HenyeyGreenstein g:"+std::to_string(g);
    }

  private:
    const Float g;
};

//// MediumInterface Declarations
//struct MediumInterface {
//    MediumInterface() : inside(nullptr), outside(nullptr) {}
//    // MediumInterface Public Methods
//    MediumInterface(const Medium *medium) : inside(medium), outside(medium) {}
//    MediumInterface(const Medium *inside, const Medium *outside)
//        : inside(inside), outside(outside) {}
//    bool IsMediumTransition() const { return inside != outside; }
//    const Medium *inside, *outside;
//};


