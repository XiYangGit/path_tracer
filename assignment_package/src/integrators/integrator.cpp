#include "integrator.h"

void Integrator::run()
{
    Render();
}

void Integrator::Render()
{
    // Compute the bounds of our sample, clamping to screen's max bounds if necessary
    // Instantiate a FilmTile to store this thread's pixel colors
    std::vector<Point2i> tilePixels = bounds.GetPoints();
    Color3f beta(1.0f);
    // For every pixel in the FilmTile:
    for(Point2i pixel : tilePixels)
    {
//        ///Uncomment this to debug a particular pixel within this tile
        if(pixel.x == 200 && pixel.y == 300)
        {
            continue;
        }
        //std::cout<<pixel.x<<", "<<pixel.y<<std::endl;
        Color3f pixelColor(0.f);
        // Ask our sampler for a collection of stratified samples, then raycast through each sample
        std::vector<Point2f> pixelSamples = sampler->GenerateStratifiedSamples();
        int debugi=0;
        for(Point2f sample : pixelSamples)
        {
            debugi++;
            //std::cout<<debugi<<std::endl;
            sample = sample + Point2f(pixel); // _sample_ is [0, 1), but it needs to be translated to the pixel's origin.
            // Generate a ray from this pixel sample
            Ray ray=camera->Raycast(sample);
            //Ray ray=scene->camera.Raycast(sample);
            if(useLensCamera)
            {Ray castRay = scene->lensCamera.Raycast(sample,sampler->Get2D());
//                ray.origin=castRay.origin;
//                ray.direction=castRay.direction;
                ray=Ray(castRay);
            }
            //Ray ray=scene->lensCamera.Raycast(sample,sampler->Get2D());    //uncomment this to use lens camera
            // Get the L (energy) for the ray by calling Li(ray, scene, tileSampler, arena)
            // Li is implemented by Integrator subclasses, like DirectLightingIntegrator
            Color3f L = Li(ray, *scene, sampler, recursionLimit,beta);
            // Accumulate color in the pixel
            pixelColor += L;
        }
        // Average all samples' energies
        pixelColor /= pixelSamples.size();
        film->SetPixelColor(pixel, glm::clamp(pixelColor, 0.f, 1.f));
    }
    //We're done here! All pixels have been given an averaged color.
}


void Integrator::ClampBounds()
{
    Point2i max = bounds.Max();
    max = Point2i(std::min(max.x, film->bounds.Max().x), std::min(max.y, film->bounds.Max().y));
    bounds = Bounds2i(bounds.Min(), max);
}
float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    if(fabs(nf * fPdf + ng * gPdf)<FLT_EPSILON)
        return 0.0f;
    return (float)(nf * fPdf) / (nf * fPdf + ng * gPdf);
    //return 0.f;
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO

    Float f = nf * fPdf, g = ng * gPdf;
    if(fabs(f * f + g * g)<FLT_EPSILON)
        return 0.0f;
    return (float)(f * f) / (f * f + g * g);
    //return 0.f;
}
