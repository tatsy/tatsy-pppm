#ifndef _PROGRESSIVE_PHOTON_MAPPING_H_
#define _PROGRESSIVE_PHOTON_MAPPING_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef PROGRESSIVe_PHOTON_MAPPING_EXPORT
        #define PROGRESSIVe_PHOTON_MAPPING_DLL __declspec(dllexport)
    #else
        #define PROGRESSIVe_PHOTON_MAPPING_DLL __declspec(dllimport)
    #endif
#else
    #define PROGRESSIVe_PHOTON_MAPPING_DLL
#endif


#include "image.h"
#include "scene.h"
#include "perspective_camera.h"

#include "random.h"
#include "random_sequence.h"

#include "hash_grid.h"
#include "render_parameters.h"

class PROGRESSIVe_PHOTON_MAPPING_DLL ProgressivePhotonMapping {
private:
    struct RenderPoint : Vector3D {
        Vector3D normal;
        Vector3D flux;
        Vector3D weight;
        Vector3D emission;
        double coeff;
        int pixelX, pixelY;
        double r2;
        int n;

        explicit RenderPoint(const Vector3D& v = Vector3D())
            : Vector3D(v)
            , normal()
            , flux()
            , weight()
            , emission()
            , coeff(0.0)
            , pixelX(-1)
            , pixelY(-1)
            , r2(0.0)
            , n(0)
        {
        }

        RenderPoint(const RenderPoint& rp)
            : Vector3D()
            , normal()
            , flux()
            , weight()
            , emission()
            , coeff(0.0)
            , pixelX(-1)
            , pixelY(-1)
            , r2(0.0)
            , n(0)
        {
            this->operator=(rp);
        }

        RenderPoint& operator=(const RenderPoint& rp) {
            Vector3D::operator=(rp);
            this->normal = rp.normal;
            this->flux = rp.flux;
            this->weight = rp.weight;
            this->emission = rp.emission;
            this->coeff = rp.coeff;
            this->pixelX = rp.pixelX;
            this->pixelY = rp.pixelY;
            this->r2 = rp.r2;
            this->n = rp.n;
            return *this;
        }

        void setPosition(const Vector3D& v) {
            Vector3D::operator=(v);
        }
    };

    HashGrid<RenderPoint*> hashgrid;
    static const double ALPHA;

    Image _result;

public:
    ProgressivePhotonMapping();
    ~ProgressivePhotonMapping();

public:
    void render(const Scene& scene, const Camera& camera, const RenderParameters& params);

    inline const Image& result() const { return _result; }

private:
    void constructHashGrid(std::vector<RenderPoint>& rpoints, const int imageW, const int imageH);
    void traceRays(const Scene& scene, const Camera& camera, Random& rand, std::vector<RenderPoint>* rpoints);
    void tracePhotons(const Scene& scene, Random& rand, int photons, const int bounceLimit = 64);
    void executePathTracing(const Scene& scene, const Camera& camera, RandomSequence& rseq, RenderPoint* rp, const int bounceLimit = 64);
};

#endif  // _PROGRESSIVE_PHOTON_MAPPING_H_
