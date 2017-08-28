#pragma once
#include "light.h"
#include "scene/bounds.h"
class ProjectionLight: public Light
{
public:
    ProjectionLight(const Transform &t,
                    const std::shared_ptr<QImage> &projectionMap,
                    const Point3f &pLight,const Color3f &I,
                    Float fov
                    );

    virtual Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                                Vector3f *wi, Float *pdf) const;
    Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                                Vector3f *wi, Float *pdf,Intersection *lightsample) const;
    virtual bool getPosition(Point3f *wi) const;
    virtual Float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;

    Color3f Projection(const Vector3f &w) const;
    static Color3f GetImageColor(const Point2f &uv_coord, const QImage* const image)
    {
        if(image)
        {
            int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
            int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);
            QColor color = image->pixel(X, Y);
            return Color3f(color.red(), color.green(), color.blue())/255.0f;
        }
        return Color3f(1.f, 1.f, 1.f);
    }

private:
  //std::unique_ptr<MIPMap<RGBSpectrum>> projectionMap;
  std::shared_ptr<QImage> projectionMap;
  const Point3f pLight;
  const Color3f I;
  Matrix4x4 lightProjection;
  Float hither, yon;
  Bounds2f screenBounds;
  Float cosTotalWidth;
};
Matrix4x4 Scale(Float x, Float y, Float z);
Matrix4x4 Perspective(Float fov, Float n, Float f);
