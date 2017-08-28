#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H
#include "light.h"

class SpotLight:public Light
{
public:
    SpotLight(const Transform &t,Point3f pLight,const Color3f &I, Float totalWidth, Float falloffStart):
        Light(t),pLight(pLight),I(I),cosTotalWidth(std::cos(glm::radians(totalWidth))),
        cosFalloffStart(std::cos(glm::radians(falloffStart))){}
    virtual Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                                Vector3f *wi, Float *pdf) const;

    virtual bool getPosition(Point3f *wi) const;
    virtual Float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;
    Float Falloff(const Vector3f &w) const;
    Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                                Vector3f *wi, Float *pdf,Intersection *lightsample) const;
private:
    const Point3f pLight;
    const Color3f I;
    const Float cosTotalWidth, cosFalloffStart;
};

#endif // SPOTLIGHT_H
