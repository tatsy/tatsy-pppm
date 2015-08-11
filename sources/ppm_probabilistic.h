#ifndef _PPM_PROBABILISTIC_H_
#define _PPM_PROBABILISTIC_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef PPM_PROBABILISTIC_EXPORT
        #define PPM_PROBABILISTIC_DLL __declspec(dllexport)
    #else
        #define PPM_PROBABILISTIC_DLL __declspec(dllimport)
    #endif
#else
    #define PPM_PROBABILISTIC_DLL
#endif

#include "scene.h"
#include "perspective_camera.h"
#include "render_parameters.h"
#include "image.h"
#include "subsurface_integrator.h"
#include "readonly_interface.h"
#include "photon_map.h"
#include "halton.h"

class PPM_PROBABILISTIC_DLL ProgressivePhotonMappingProb : private IReadOnly {
private:
    static const double ALPHA;
    Image _result;
    SubsurfaceIntegrator* _integrator;
    double _radius;
    PhotonMap _photonMap;

public:
    ProgressivePhotonMappingProb();
    ~ProgressivePhotonMappingProb();

    void render(const Scene& scene, const Camera& camera, const RenderParameters& params);

private:
    void tracePhotons(const Scene& scene, const Camera& camera, const RenderParameters& params, Halton* hals, int bounceLimit = 64);
    void traceRays(Image* buffer, const Scene& scene, const Camera& camera, const RenderParameters& params, Halton* hals) const;
    Vector3D executePathTracing(const Scene& scene, const Camera& camera, const RenderParameters& params, int pixelX, int pixelY, RandomSequence& rseq, int bounceLimit = 64) const;
    Vector3D radiance(const Scene& scene, const Ray& ray, const RenderParameters& params, RandomSequence& rseq, int bounces, int bounceLimit = 64) const;
};

#endif  // _PPM_PROBABILISTIC_H_