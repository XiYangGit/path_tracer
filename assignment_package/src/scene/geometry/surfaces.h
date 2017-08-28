#pragma once
#include <scene/geometry/shape.h>


class Surface : public Shape
{
public:
    Surface(QString t):type(t){}
    virtual bool Intersect(const Ray &ray, Intersection *isect) const;
    virtual Point2f GetUVCoordinates(const Point3f &point) const;
    virtual void ComputeTBN(const Point3f& P, Normal3f* nor, Vector3f* tan, Vector3f* bit) const;

    virtual float Area() const;

    // Sample a point on the surface of the shape and return the PDF with
    // respect to area on the surface.
    virtual Intersection Sample(const Point2f &xi, Float *pdf) const;

    Bounds3f WorldBound() const;

    void create();


    Float SDF(const Point3f &p) const;
    Normal3f estimateNormal(const Point3f &p) const;
    virtual void InitializeIntersection(Intersection* isect, float t, Point3f pLocal) const;
    QString type;

};
