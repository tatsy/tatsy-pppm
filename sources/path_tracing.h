#ifndef _PATH_TRACING_H_
#define _PATH_TRACING_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef PATH_TRACING_EXPORT
        #define PATH_TRACING_DLL __declspec(dllexport)
    #else
        #define PATH_TRACING_DLL __declspec(dllimport)
    #endif
#else
    #define PATH_TRACING_DLL
#endif

#include "image.h"
#include "scene.h"
#include "perspective_camera.h"
#include "render_parameters.h"
#include "subsurface_integrator.h"

class PATH_TRACING_DLL PathTracing {
private:
    Image _result;
    SubsurfaceIntegrator* _integrator;

public:
    PathTracing();
    ~PathTracing();

    void render(const Scene& scene, const Camera& camera, const RenderParameters& params, bool enableBSSRDF = false);

private:
    Vector3D executePathTracing(const Scene& scene, const Camera& camera, double pixelX, double pixelY, RandomSequence& rseq, int maxBounces = 32) const;
};

#endif  // _PATH_TRACING_H_
