#pragma once
#include <globals.h>
#include <scene/materials/bsdf.h>
#include <raytracing/ray.h>
#include <scene/geometry/primitive.h>
#include <QList>
#include <scene/materials/medium.h>
class Material;
class BSDF;
class Primitive;
class PhaseFunction;
class Medium;
//struct MediumInterface;
struct MediumInterface {
    MediumInterface() : inside(nullptr), outside(nullptr) {}
    // MediumInterface Public Methods
    MediumInterface(const Medium *medium) : inside(medium), outside(medium) {}
    MediumInterface(const Medium *inside, const Medium *outside)
        : inside(inside), outside(outside) {}
    bool IsMediumTransition() const { return inside != outside; }
    const Medium *inside, *outside;
};

class Intersection
{
public:
    Intersection();
    Intersection(MediumInterface &mediumInterface,std::shared_ptr<PhaseFunction> phase);
    Intersection(const Medium *medium,std::shared_ptr<PhaseFunction> phase);
    Intersection(std::shared_ptr<PhaseFunction> phase);
    Intersection(Point3f p):point(p),phase(nullptr){}
    // Ask _objectHit_ to produce a BSDF
    // based on other data stored in this
    // Intersection, e.g. the surface normal
    // and UV coordinates
    bool ProduceBSDF();

    // Returns the light emitted from this Intersection
    // along _wo_, provided that this Intersection is
    // on the surface of an AreaLight. If not, then
    // zero light is returned.
    Color3f Le(const Vector3f& wo) const;

    // Instantiate a Ray that originates from this Intersection and
    // travels in direction d.
    Ray SpawnRay(const Vector3f &d) const;

    Point3f point;          // The place at which the intersection occurred
    Normal3f normalGeometric; // The surface normal at the point of intersection, NO alterations like normal mapping applied
    Point2f uv;             // The UV coordinates computed at the intersection
    float t;                  // The parameterization for the ray (in world space) that generated this intersection.
                              // t is equal to the distance from the point of intersection to the ray's origin if the ray's direction is normalized.
    Primitive const * objectHit;     // The object that the ray intersected, or nullptr if the ray hit nothing.
    std::shared_ptr<BSDF> bsdf;// The Bidirection Scattering Distribution Function found at the intersection.

    Vector3f tangent, bitangent; // World-space vectors that form an orthonormal basis with the surface normal.




    // MediumInteraction Public Data
    bool IsSurfaceIntersection;
    //const PhaseFunction *phase;
    std::shared_ptr<PhaseFunction> phase;
    MediumInterface mediumInterface;
    //members for medium intersection
    bool IsValid() const { return phase != nullptr; }

    Vector3f wo;

    const Medium *GetMedium(const Vector3f &w) const {
            return glm::dot(w, normalGeometric) > 0 ? mediumInterface.outside : mediumInterface.inside;
        }
        const Medium *GetMedium() const {
            if(mediumInterface.inside== mediumInterface.outside)
            return mediumInterface.inside;
            else
                return nullptr;
        }
        Ray SpawnRayTo(const Intersection &it) const;
        bool IsSurfaceInteraction() const {
        return normalGeometric != Normal3f();
        }

};

inline Point3f OffsetRayOrigin(const Point3f &p, const float &rayelip,
const Normal3f &n, const Vector3f &w) {
    Vector3f pError(rayelip);
Float d = glm::dot(glm::abs(n), pError);
Vector3f offset = d * Vector3f(n);
if (glm::dot(w, n) < 0)
offset = -offset;
Point3f po = p + offset;
return po;
}
