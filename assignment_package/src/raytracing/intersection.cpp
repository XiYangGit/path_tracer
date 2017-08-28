#include <raytracing/intersection.h>

Intersection::Intersection():
    point(Point3f(0)),
    normalGeometric(Normal3f(0)),
    uv(Point2f(0)),
    t(-1),
    objectHit(nullptr),
    bsdf(nullptr),
    tangent(0.f), bitangent(0.f),
    IsSurfaceIntersection(true),phase(nullptr),mediumInterface(MediumInterface())
{}
Intersection::Intersection(std::shared_ptr<PhaseFunction> phase):
    point(Point3f(0)),
    normalGeometric(Normal3f(0)),
    uv(Point2f(0)),
    t(-1),
    objectHit(nullptr),
    bsdf(nullptr),
    tangent(0.f), bitangent(0.f),
    IsSurfaceIntersection(false),phase(phase),mediumInterface(MediumInterface())
{}
Intersection::Intersection(MediumInterface &mediumInterface, std::shared_ptr<PhaseFunction> phase):
    point(Point3f(0)),
    normalGeometric(Normal3f(0)),
    uv(Point2f(0)),
    t(-1),
    objectHit(nullptr),
    bsdf(nullptr),
    tangent(0.f), bitangent(0.f),
    IsSurfaceIntersection(false),phase(phase),mediumInterface(mediumInterface)
{}
Intersection::Intersection(const Medium *medium, std::shared_ptr<PhaseFunction> phase):
    point(Point3f(0)),
    normalGeometric(Normal3f(0)),
    uv(Point2f(0)),
    t(-1),
    objectHit(nullptr),
    bsdf(nullptr),
    tangent(0.f), bitangent(0.f),
    IsSurfaceIntersection(false),phase(phase),mediumInterface(MediumInterface(medium))
{}



bool Intersection::ProduceBSDF()
{
    return objectHit->ProduceBSDF(this);
}

Color3f Intersection::Le(const Vector3f &wo) const
{
    const AreaLight* light = objectHit->GetAreaLight();
    return light ? light->L(*this, wo) : Color3f(0.f);
//    if(objectHit->areaLight)
//        return objectHit->areaLight->L(*this,wo);
//    else
//        return Color3f(0.0f);
}

Ray Intersection::SpawnRay(const Vector3f &d) const
{
    Vector3f originOffset = normalGeometric * RayEpsilon;
    // Make sure to flip the direction of the offset so it's in
    // the same general direction as the ray direction
    originOffset = (glm::dot(d, normalGeometric) > 0) ? originOffset : -originOffset;
    Point3f o(this->point + originOffset);
    return Ray(o, d,GetMedium(d));
}
Ray Intersection::SpawnRayTo(const Intersection &it) const {
    Point3f origin = OffsetRayOrigin(point, RayEpsilon, normalGeometric, it.point - point);
    Point3f target = OffsetRayOrigin(it.point, RayEpsilon, it.normalGeometric, origin - it.point);
    Vector3f d = target - origin;
    return Ray(origin, d, 1 - ShadowEpsilon,  GetMedium(d));
//        //Point3f origin = OffsetRayOrigin(p, pError, n, it.p - p);
//        Vector3f originOffset = normalGeometric * RayEpsilon;
//        // Make sure to flip the direction of the offset so it's in
//        // the same general direction as the ray direction
//        originOffset = (glm::dot(it.point - this->point, normalGeometric) > 0) ? originOffset : -originOffset;
//        Point3f origin(this->point + originOffset);

//        //Point3f target = OffsetRayOrigin(it.p, it.pError, it.n, origin - it.p);
//        Vector3f originOffset2 = it.normalGeometric * RayEpsilon;
//        // Make sure to flip the direction of the offset so it's in
//        // the same general direction as the ray direction
//        originOffset2 = (glm::dot(origin - it.point, it.normalGeometric) > 0) ? originOffset2 : -originOffset2;
//        Point3f target =(it.point + originOffset2);
//        Vector3f d = target - origin;
//        return Ray(origin, d, 1 - ShadowEpsilon, GetMedium(d));
    }
