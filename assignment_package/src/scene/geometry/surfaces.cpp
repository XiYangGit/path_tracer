#include "surfaces.h"
#define MAX_MARCHING_STEPS  500
#define EPSILON 0.0001
#define start 0.0
#define end 10000.0

bool Surface::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    float depth = start;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = SDF(r_loc.origin + depth * r_loc.direction);
        if (dist < EPSILON) {
            // We're inside the scene surface!
//            if(depth>EPSILON){
//            glm::vec4 P = glm::vec4(r_loc.origin + depth*r_loc.direction, 1);
//            InitializeIntersection(isect,depth,Point3f(P));
//            return true;}
            //if the point goes inside the geometry, trace back to surface
            for(float j=0.0f;j<0.1f;j+=0.001f)
            {
                depth-=j;
                float j_sdf=SDF(r_loc.origin + depth * r_loc.direction);
                 if(j_sdf>0.0f)
                     break;

            }
            glm::vec4 P = glm::vec4(r_loc.origin + depth*r_loc.direction, 1);
            InitializeIntersection(isect,depth,Point3f(P));
            if(ray.tMax>depth)
                ray.tMax=depth;
            else
                return false;
            return true;
        }
        // Move along the view ray
        depth += dist;

        if (depth >= end) {
            // Gone too far; give up
            return false;
        }
    }
    return false;
}
void Surface::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * estimateNormal(P));
    //TODO: Compute tangent and bitangent
    Vector3f tangent;
    Vector3f bitangent;
    CoordinateSystem(estimateNormal(P),&tangent,&bitangent);
    //*tan=glm::normalize(transform.T3()*tangent);
    //*bit=glm::normalize(transform.T3()*bitangent);
    *tan=tangent;
    *bit=bitangent;
}
Float Surface::SDF(const Point3f &p) const
{
    float sdf(0.0f);
    if(QString::compare(type, QString("Torus")) == 0){
    float xx=p.x*p.x;
    float yy=p.y*p.y;
    float zz=p.z*p.z;
    float c=3,a=1;  //torus parameters
    float sdf=sqrt(powf(c-sqrt(xx+yy),2.0)+zz)-a;
    return sdf;
    }

    else if(QString::compare(type, QString("Ellipsoid")) == 0){

        float xx=p.x*p.x;
        float yy=p.y*p.y;
        float zz=p.z*p.z;
        float cc=9,aa=1,bb=4;  //ellipsoid parameters
        sdf=sqrt(xx/aa+yy/bb+zz/cc)-1;
        return sdf;
    }
    else if(QString::compare(type, QString("Heart")) == 0){
       float xx=p.x*p.x;
       float yy=p.y*p.y;
       float zz=p.z*p.z;
       float z3=zz*p.z;
       sdf=powf(xx+2.25f*yy+zz-1.0f,3.0f)-xx*z3-yy*z3*0.1125f;
       if(sdf<0.0f)
           sdf=-powf(-sdf,1.0f/6.0f);
       else
           sdf=powf(sdf,1.0f/6.0f);
       return sdf;}
    else if(QString::compare(type, QString("ToothSurface")) == 0){
        float xx=p.x*p.x;
        float yy=p.y*p.y;
        float zz=p.z*p.z;
        sdf=xx*xx+yy*yy+zz*zz-(xx+yy+zz);
        if(sdf<0.0f)
           sdf=-powf(-sdf,1.0f/4.0f);
        else
           sdf=powf(sdf,1.0f/4.0f);
        return sdf;
    }
    else
    {
       //default case
        sdf= glm::length(p)-1;  //default case: sphere
        return sdf;

    }

    return sdf;
}
Normal3f Surface::estimateNormal(const Point3f &p) const
{
    Normal3f nor;
    float ep=0.000001f;
    nor[0]=SDF(Point3f(p.x + ep, p.y, p.z)) - SDF(Point3f(p.x - ep, p.y, p.z));
    nor[1]=SDF(Point3f(p.x, p.y + ep, p.z)) - SDF(Point3f(p.x, p.y - ep, p.z));
    nor[2]=SDF(Point3f(p.x, p.y, p.z + ep)) - SDF(Point3f(p.x, p.y, p.z - ep));
    return glm::normalize(nor);
}
Point2f Surface::GetUVCoordinates(const Point3f &point) const
{
    return Point2f(0.0f);
}
//void Surface::create(){}
float Surface::Area()const {return 0.0f;}
Intersection Surface::Sample(const Point2f &xi, Float *pdf) const{ return Intersection();}
Bounds3f Surface::WorldBound() const{
    Point3f min(-3,-3,-3);
    Point3f max(3,3,3);
    return Bounds3f(min,max).Apply(transform);
}
void Surface::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->point+=isect->normalGeometric*(float)EPSILON;
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}
