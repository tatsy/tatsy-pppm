#ifndef _SUBSURFACE_INTEGRATOR_H_
#define _SUBSURFACE_INTEGRATOR_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef SUBSURFACE_INTEGRATOR_EXPORT
        #define SUBSURFACE_INTEGRATOR_DLL __declspec(dllexport)
    #else
        #define SUBSURFACE_INTEGRATOR_DLL __declspec(dllimport)
    #endif
#else
    #define SUBSURFACE_INTEGRATOR_DLL
#endif

#include "readonly_interface.h"

#include "bbox.h"
#include "scene.h"
#include "render_parameters.h"
#include "bsdf.h"
#include "photon.h"
#include "photon_map.h"

struct IrradiancePoint {
    Vector3D pos;
    Vector3D normal;
    double area;
    Vector3D irad;

    IrradiancePoint()
        : pos()
        , normal()
        , area(0.0)
        , irad()
    {
    }

    IrradiancePoint(const Vector3D& pos_, const Vector3D& normal_, const double area_, const Vector3D& irad_)
        : pos(pos_)
        , normal(normal_)
        , area(area_)
        , irad(irad_)
    {
    }

    IrradiancePoint(const IrradiancePoint& p)
        : pos()
        , normal()
        , area(0.0)
        , irad()
    {
        this->operator=(p);
    }

    IrradiancePoint& operator=(const IrradiancePoint& p) {
        this->pos = p.pos;
        this->normal = p.normal;
        this->area = p.area;
        this->irad = p.irad;
        return *this;
    }
};

class SUBSURFACE_INTEGRATOR_DLL  SubsurfaceIntegrator : private IReadOnly {
private:
    struct OctreeNode {
        IrradiancePoint pt;
        BBox bbox;
        OctreeNode* children[8];
        bool isLeaf;

        OctreeNode()
            : pt()
            , bbox()
            , isLeaf(false)
        {
            for (int i = 0; i < 8; i++) {
                children[i] = NULL;
            }
        }
                
    };

    class Octree {
    private:
        OctreeNode* _root;
        int* _numCopies;
        SubsurfaceIntegrator* _parent;

    public:
        Octree();
        ~Octree();

        Octree(const Octree& octree);
        Octree& operator=(const Octree& octree);

        void construct(SubsurfaceIntegrator* parent, std::vector<IrradiancePoint>& ipoints);

        Vector3D iradSubsurface(const Vector3D& pos, const BSSRDF& Rd) const;

    private:
        void release();
        void deleteNode(OctreeNode* node);
        OctreeNode* constructRec(std::vector<IrradiancePoint>& pointers, const BBox& bbox);

        Vector3D iradSubsurfaceRec(OctreeNode* node, const Vector3D& pos, const BSSRDF& Rd) const;
    };


private:
    PhotonMap photonMap;
    Octree octree;
    double dA;
    double _maxError;

public:
    SubsurfaceIntegrator();
    ~SubsurfaceIntegrator();

    void initialize(const Scene& scene, const RenderParameters& params, const double areaRadius, const double maxError = 0.05);

    void buildOctree(const std::vector<Vector3D>& points, const std::vector<Vector3D>& normals, const RenderParameters& params);

    Vector3D irradiance(const Vector3D& p, const BSDF& bsdf) const;

private:
    void buildPhotonMap(const Scene& scene, const int numPhotons, const int bounceLimit);

    Vector3D irradianceWithPM(const Vector3D& p, const Vector3D& n, const RenderParameters& params) const;
};


#endif  // _SPICA_SUBSURFACE_INTEGRATOR_H_
