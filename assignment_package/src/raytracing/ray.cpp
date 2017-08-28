#include <raytracing/ray.h>

Ray::Ray(const Point3f &o, const Vector3f &d):
    origin(o),
    direction(d),
    tMax(INFINITY),medium(nullptr)
{}

Ray::Ray(const glm::vec4 &o, const glm::vec4 &d):
    Ray(Point3f(o), Vector3f(d))
{}

Ray::Ray(const Ray &r):
    origin(r.origin),
    direction(r.direction),
    tMax(r.tMax),medium(r.medium)
{}
Ray::Ray(const Point3f &o, const Vector3f &d, const Medium *medium):
    origin(o),direction(d),tMax(INFINITY),medium(medium)
{}
Ray::Ray(const Point3f &o, const Vector3f &d, float tMax, const Medium *medium):
    origin(o),direction(d),tMax(tMax),medium(medium)
{}
Ray Ray::GetTransformedCopy(const Matrix4x4 &T) const
{

    glm::vec4 o = glm::vec4(origin, 1);
    glm::vec4 d = glm::vec4(direction, 0);

    o = T * o;
    d = T * d;

    return Ray(o, d);
}
