#include "bounds.h"

bool Bounds3f::IntersectP(const Ray &ray, const Vector3f &invDir,
                       const int dirIsNeg[3]) const {
    const Bounds3f &bounds = *this;
    // Check for ray intersection against $x$ and $y$ slabs
    Float tMin = (bounds[dirIsNeg[0]].x - ray.origin.x) * invDir.x;
    Float tMax = (bounds[1 - dirIsNeg[0]].x - ray.origin.x) * invDir.x;
    Float tyMin = (bounds[dirIsNeg[1]].y - ray.origin.y) * invDir.y;
    Float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.origin.y) * invDir.y;

    // Update _tMax_ and _tyMax_ to ensure robust bounds intersection
    tMax *= 1 + 2 * gamma(3);
    tyMax *= 1 + 2 * gamma(3);
    if (tMin > tyMax || tyMin > tMax) return false;
    if (tyMin > tMin) tMin = tyMin;
    if (tyMax < tMax) tMax = tyMax;

    // Check for ray intersection against $z$ slab
    Float tzMin = (bounds[dirIsNeg[2]].z - ray.origin.z) * invDir.z;
    Float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.origin.z) * invDir.z;

    // Update _tzMax_ to ensure robust bounds intersection
    tzMax *= 1 + 2 * gamma(3);
    if (tMin > tzMax || tzMin > tMax) return false;
    if (tzMin > tMin) tMin = tzMin;
    if (tzMax < tMax) tMax = tzMax;
    //return (tMax > 0);
    return (tMin < ray.tMax) && (tMax > 0);
}

bool Bounds3f::Intersect(const Ray &r, float* t) const
{
    //TODO
    //return false;
    float t_n = -1000000;
    float t_f = 1000000;
    for(int i = 0; i < 3; i++){
        //Ray parallel to slab check
        if(r.direction[i] == 0){
            if(r.origin[i] < min[i] || r.origin[i] > max[i]){
                return false;
            }
        }
        //If not parallel, do slab intersect check
        float t0 = (min[i] - r.origin[i])/r.direction[i];
        float t1 = (max[i] - r.origin[i])/r.direction[i];
        if(t0 > t1){
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        if(t0 > t_n){
            t_n = t0;
        }
        if(t1 < t_f){
            t_f = t1;
        }
    }
    if(t_n < t_f)
    {
        *t = t_n > 0 ? t_n : t_f;
        if(*t < 0)
        {
            if(InsideBox(*this,r.origin))
                return true;
            else
                return false;
        }
        return true;
    }
    else{//If t_near was greater than t_far, we did not hit the cube
        return false;
    }
}

Bounds3f Bounds3f::Apply(const Transform &tr)
{
    //TODO
    Point3f pt[8];
    pt[0]=Point3f(min.x,min.y,min.z);
    pt[1]=Point3f(min.x,max.y,min.z);
    pt[2]=Point3f(max.x,max.y,min.z);
    pt[3]=Point3f(max.x,min.y,min.z);
    pt[4]=Point3f(min.x,min.y,max.z);
    pt[5]=Point3f(min.x,max.y,max.z);
    pt[6]=Point3f(max.x,max.y,max.z);
    pt[7]=Point3f(max.x,min.y,max.z);
    std::vector<glm::vec4> transformedPt;
    for(int i=0;i<8;i++)
    {
        transformedPt.push_back(tr.T()*glm::vec4(pt[i],1.0f));
    }
    Bounds3f newBounds=Bounds3f(Point3f(transformedPt[0]));
    for(int i=0;i<8;i++)
    {
        newBounds=Union(newBounds,transformedPt[i]);
    }
    return newBounds;
       //return Bounds3f();
}

float Bounds3f::SurfaceArea() const
{
    //TODO
    float L=max.x-min.x;
    float W=max.z-min.z;
    float H=max.y-min.y;
    return 2.0f*(L*W+W*H+H*L);
    //return 0.f;
}

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2)
{
    return Bounds3f(Point3f(std::min(b1.min.x, b2.min.x),
                            std::min(b1.min.y, b2.min.y),
                            std::min(b1.min.z, b2.min.z)),
                    Point3f(std::max(b1.max.x, b2.max.x),
                            std::max(b1.max.y, b2.max.y),
                            std::max(b1.max.z, b2.max.z)));
}

Bounds3f Union(const Bounds3f& b1, const Point3f& p)
{
    return Bounds3f(Point3f(std::min(b1.min.x, p.x),
                            std::min(b1.min.y, p.y),
                            std::min(b1.min.z, p.z)),
                    Point3f(std::max(b1.max.x, p.x),
                            std::max(b1.max.y, p.y),
                            std::max(b1.max.z, p.z)));
}

Bounds3f Union(const Bounds3f& b1, const glm::vec4& p)
{
    return Union(b1, Point3f(p));
}
bool InsideBox(const Bounds3f& b1, const Point3f& p)
{
    for(int i=0;i<3;i++)
    {
        if(p[i] < b1.min[i] || p[i] > b1.max[i]){
            return false;
        }
    }
    return true;
}
