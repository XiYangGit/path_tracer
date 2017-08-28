#pragma once
#include <globals.h>
#include "scene/materials/medium.h"

class Medium;
class Ray
{
public:
    Ray(const Point3f &o, const Vector3f &d);
    Ray(const glm::vec4 &o, const glm::vec4 &d);
    Ray(const Ray &r);
    Ray(const Point3f &o, const Vector3f &d,const Medium *medium);
    Ray(const Point3f &o, const Vector3f &d,float tMax,const Medium *medium);
    //Return a copy of this ray that has been transformed
    //by the input transformation matrix.
    Ray GetTransformedCopy(const Matrix4x4 &T) const;

    Point3f origin;
    Vector3f direction;
    mutable Float tMax;
    const Medium *medium;

};
