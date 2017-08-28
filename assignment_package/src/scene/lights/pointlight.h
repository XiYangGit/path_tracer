#pragma once

#include "light.h"

class PointLight : public Light
{
public:
    PointLight(const Transform &t,const Point3f &pLight,const Color3f& I):Light(t),pLight(pLight),I(I){}
    virtual Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                                Vector3f *wi, Float *pdf) const;

    virtual bool getPosition(Point3f *wi) const;
    virtual Float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;
    Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                                Vector3f *wi, Float *pdf,Intersection *lightsample) const;
    const Point3f pLight;
    const Color3f I;
};

#pragma once
