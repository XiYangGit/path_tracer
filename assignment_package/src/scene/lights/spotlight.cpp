#include "spotlight.h"

Color3f SpotLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const
{
    *wi = glm::normalize(pLight - ref.point);
    *pdf = 1.0f;

    return I * Falloff(-*wi) / glm::distance2(pLight, ref.point);
}

Color3f SpotLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf, Intersection *lightsample) const
{
            *wi = glm::normalize(pLight - ref.point);
            *pdf = 1.0f;
            *lightsample = Intersection(pLight);
            return I * Falloff(-*wi) / glm::distance2(pLight, ref.point);
}
Float SpotLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 0.0f;
}
bool SpotLight::getPosition(Point3f *wi) const
{
    *wi=pLight;
    return true;
}
Float SpotLight::Falloff(const Vector3f &w) const
{
    Vector3f wl = glm::normalize(Vector3f(transform.invT()*glm::vec4(w,0)));  //not sure 0 or 1
    Float cosTheta = wl.z;
    if (cosTheta < cosTotalWidth) return 0;
    if (cosTheta > cosFalloffStart) return 1;
    // Compute falloff inside spotlight cone
    Float delta =
            (cosTheta - cosTotalWidth) / (cosFalloffStart - cosTotalWidth);
    return (delta * delta) * (delta * delta);
}
