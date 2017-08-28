#pragma once

#include <globals.h>

#include <raytracing/ray.h>
#include <openGL/drawable.h>
#include "materials/medium.h"

//A perspective projection camera
//Receives its eye position and reference point from the scene XML file
class Medium;

class Camera : public Drawable
{
public:
    Camera();
    Camera(Medium*);
    Camera(unsigned int w, unsigned int h);
    Camera(unsigned int w, unsigned int h, const Vector3f &e, const Vector3f &r, const Vector3f &worldUp);
    Camera(const Camera &c);
    Camera(unsigned int w, unsigned int h, const Vector3f &e, const Vector3f &r, const Vector3f &worldUp,Medium* medium);
//~Camera(){
//        if(medium!=nullptr)
//        delete medium;
//    medium=nullptr;}
    void CopyAttributes(const Camera &c);

    float fovy;
    unsigned int width, height;  // Screen dimensions
    float near_clip;  // Near clip plane distance
    float far_clip;  // Far clip plane distance

    //Computed attributes
    float aspect;

    Point3f  eye,      //The position of the camera in world space
             ref;      //The point in world space towards which the camera is pointing
    Vector3f look,     //The normalized vector from eye to ref. Is also known as the camera's "forward" vector.
             up,       //The normalized vector pointing upwards IN CAMERA SPACE. This vector is perpendicular to LOOK and RIGHT.
             right,    //The normalized vector pointing rightwards IN CAMERA SPACE. It is perpendicular to UP and LOOK.
             world_up, //The normalized vector pointing upwards IN WORLD SPACE. This is primarily used for computing the camera's initial UP vector.
             V,        //Represents the vertical component of the plane of the viewing frustum that passes through the camera's reference point. Used in Camera::Raycast.
             H;        //Represents the horizontal component of the plane of the viewing frustum that passes through the camera's reference point. Used in Camera::Raycast.

    Matrix4x4 GetViewProj() const;

    void RecomputeAttributes();

    Ray Raycast(const Point2f &pt) const;         //Creates a ray in 3D space given a 2D point on the screen, in screen coordinates.
    Ray Raycast(float x, float y) const;            //Same as above, but takes two floats rather than a vec2.
    Ray RaycastNDC(float ndc_x, float ndc_y) const; //Creates a ray in 3D space given a 2D point in normalized device coordinates.

    void RotateAboutUp(float deg);
    void RotateAboutRight(float deg);

    void TranslateAlongLook(float amt);
    void TranslateAlongRight(float amt);
    void TranslateAlongUp(float amt);

    //Methods inherited from Drawable
    void create();

    virtual GLenum drawMode() const;

    //add medium
    const Medium *medium;
};
class LensBasedCamera:public Camera
{
public:
    LensBasedCamera():Camera(),lensRadius(0.0f),focalDistance(0.0f){}
    LensBasedCamera(const Camera &c,float lensRadius,float focalDistance):Camera(c),lensRadius(lensRadius),focalDistance(focalDistance){}
    LensBasedCamera(float lensRadius,float focalDistance):Camera(),lensRadius(lensRadius),focalDistance(focalDistance){}
    LensBasedCamera(unsigned int w, unsigned int h,float lensRadius,float focalDistance):Camera(w,h),lensRadius(lensRadius),focalDistance(focalDistance){}
    LensBasedCamera(unsigned int w, unsigned int h, const Vector3f &e, const Vector3f &r, const Vector3f &worldUp,float lensRadius,float focalDistance):
        Camera(w,h,e,r,worldUp),lensRadius(lensRadius),focalDistance(focalDistance){}
    LensBasedCamera(const LensBasedCamera &c):Camera(c),lensRadius(c.lensRadius),focalDistance(c.focalDistance){}
    Ray Raycast(const Point2f &pt, const Point2f &sample) const;
        //Transform CameraToScreen, RasterToCamera;
        //Transform ScreenToRaster, RasterToScreen;
    Float lensRadius, focalDistance;
};
