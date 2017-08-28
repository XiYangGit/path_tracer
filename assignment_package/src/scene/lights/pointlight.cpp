#include "pointlight.h"
Color3f PointLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const
{
        *wi = glm::normalize(pLight - ref.point);
        *pdf = 1.f;
//        *vis =
//            VisibilityTester(ref, Interaction(pLight, ref.time, mediumInterface));
        return I / glm::distance2(pLight, ref.point);
}
Float PointLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    return 0;
}
bool PointLight::getPosition(Point3f *wi) const
{
    *wi=pLight;
    return true;
}

Color3f PointLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf, Intersection *lightsample) const
{
        *wi = glm::normalize(pLight - ref.point);
        *pdf = 1.f;
        *lightsample=Intersection(pLight);
        return I / glm::distance2(pLight, ref.point);
}
