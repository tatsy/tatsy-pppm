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

class PATH_TRACING_DLL PathTracing {
private:
    Image _result;

public:
    PathTracing();
    ~PathTracing();

    void render(const Scene& scene, const Camera& camera, const RenderParameters& params);

private:
    static Vector3D executePathTracing(const Scene& scene, const Camera& camera, double pixelX, double pixelY, RandomSequence& rseq);
    static Vector3D radiance(const Scene& scene, const Ray& ray, RandomSequence& rseq, int bounces, int bounceLimit = 64, int bounceMin = 6);
};

#endif  // _PATH_TRACING_H_
