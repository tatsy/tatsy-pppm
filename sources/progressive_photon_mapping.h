#ifndef _PROGRESSIVE_PHOTON_MAPPING_H_
#define _PROGRESSIVE_PHOTON_MAPPING_H_

#include "scene.h"
#include "random.h"
#include "hash_grid.h"
#include "render_parameters.h"

class ProgressivePhotonMapping {
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

        explicit RenderPoint(const Vector3D& v)
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

public:
    ProgressivePhotonMapping();
    ~ProgressivePhotonMapping();

public:
    void render(const Scene& scene, const RenderParameters& renderParams);

private:
    void traceRays(const Scene& scene, const RenderParameters& renderParams);
    void executePathTracing(const Scene& scene, Random& rng, RenderPoint* rp, const int bounceLimit = 64);
};

#endif  // _PROGRESSIVE_PHOTON_MAPPING_H_
