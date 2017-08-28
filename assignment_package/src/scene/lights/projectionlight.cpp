#include "projectionlight.h"

ProjectionLight::ProjectionLight(const Transform &t,
                                 const std::shared_ptr<QImage> &projectionMap,
                                 const Point3f &pLight,const Color3f &I,
                                 Float fov
                                 ):Light(t),projectionMap(projectionMap),pLight(pLight),I(I)
{
    // Create _ProjectionLight_ MIP map
        Point2i resolution(400,400);
//        std::unique_ptr<RGBSpectrum[]> texels = ReadImage(texname, &resolution);
//        if (texels)
//            projectionMap.reset(new MIPMap<RGBSpectrum>(resolution, texels.get()));

        // Initialize _ProjectionLight_ projection matrix
        Float aspect =
            projectionMap ? (Float(resolution.x) / Float(resolution.y)) : 1;
        if (aspect > 1)
            screenBounds = Bounds2f(Point2f(-aspect, -1), Point2f(aspect, 1));
        else
            screenBounds =
                Bounds2f(Point2f(-1, -1 / aspect), Point2f(1, 1 / aspect));
        hither = 1e-3f;
        yon = 1e30f;
        lightProjection = Perspective(fov, hither, yon);

        // Compute cosine of cone surrounding projection directions
        Float opposite = std::tan(glm::radians(fov) / 2.f);
        Float tanDiag = opposite * std::sqrt(1 + 1 / (aspect * aspect));
        cosTotalWidth = std::cos(std::atan(tanDiag));
}
Color3f ProjectionLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const
{

       *wi = glm::normalize(pLight - ref.point);
       *pdf = 1;
       return I * Projection(-*wi) / glm::distance2(pLight, ref.point);
}

Color3f ProjectionLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf, Intersection *lightsample) const
{
        *wi = glm::normalize(pLight - ref.point);
        *pdf = 1;

        *lightsample = Intersection(pLight);
        return I * Projection(-*wi) / glm::distance2(pLight, ref.point);
}
Float ProjectionLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 0.0f;
}
bool ProjectionLight::getPosition(Point3f *wi) const
{
    *wi=pLight;
    return true;
}
Color3f ProjectionLight::Projection(const Vector3f &w) const
{
    //Vector3f wl = WorldToLight(w);
    Vector3f wl = Vector3f(transform.invT()*glm::vec4(w,0));
        // Discard directions behind projection light
        if (wl.z < hither) return Color3f(0.0f);

        // Project point onto projection plane and compute light
        //Point3f p = lightProjection(Point3f(wl.x, wl.y, wl.z));
        Point3f p = Point3f(lightProjection*glm::vec4(wl,1));
        if (!screenBounds.Inside(Point2f(p.x, p.y))) return Color3f(0.0f);
        if (!projectionMap) return Color3f(1.0f);;
        Point2f st = Point2f(screenBounds.Offset(Point2f(p.x, p.y)));
        //return Color3f(projectionMap->Lookup(st), SpectrumType::Illuminant);
        return GetImageColor(st,projectionMap.get());
}
Matrix4x4 Perspective(Float fov, Float n, Float f) {
//Perform projective divide for perspective projection 366
    Matrix4x4 persp(1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, f / (f - n), -f*n / (f - n),
    0, 0, 1, 0);
//Scale canonical perspective view to specified field of view 367
    Float invTanAng = 1 / std::tan(glm::radians(fov) / 2);
    return Scale(invTanAng, invTanAng, 1) * persp;
}
Matrix4x4 Scale(Float x, Float y, Float z) {

    glm::mat4 result(0.0f);
result[0][0]=x;
result[1][1]=y;
result[2][2]=z;
result[0][0]=1.0;
return result;
}
