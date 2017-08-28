#pragma once
#include <QList>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <scene/lights/light.h>
#include <scene/geometry/shape.h>
#include "bvh.h"
#include "samplers/sampler.h"

class Primitive;
class BVHAccel;
class Material;
class Light;
class Camera;
class LensBasedCamera;
class Scene
{
public:
    Scene();
    ~Scene();
    QList<std::shared_ptr<Primitive>> primitives;
    QList<std::shared_ptr<Material>> materials;
    QList<std::shared_ptr<Light>> lights;
    Camera camera;
    Film film;

    BVHAccel* bvh;

    QList<std::shared_ptr<Drawable>> drawables;

    void SetCamera(const Camera &c);

    void CreateTestScene();
    void Clear();

    bool Intersect(const Ray& ray, Intersection* isect) const;

    void clearBVH();

    LensBasedCamera lensCamera;

    bool IntersectP(const Ray &ray) const;
    bool IntersectTr(Ray ray, std::shared_ptr<Sampler> sampler, Intersection *isect,
                         Color3f *transmittance) const;
};
//class VisibilityTester {
//  public:
//    VisibilityTester() {}
//    // VisibilityTester Public Methods
//    VisibilityTester(const Intersection &p0, const Intersection &p1)
//        : p0(p0), p1(p1) {}
//    const Intersection &P0() const { return p0; }
//    const Intersection &P1() const { return p1; }
//    bool Unoccluded(const Scene &scene) const;
//    Color3f Tr(const Scene &scene, Sampler &sampler) const;

//  private:
//    Intersection p0, p1;
//};
