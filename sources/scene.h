#ifndef _SCENE_H_
#define _SCENE_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef SCENE_EXPORT
        #define SCENE_DLL __declspec(dllexport)
    #else
        #define SCENE_DLL __declspec(dllimport)
    #endif
#else
    #define SCENE_DLL
#endif

#include <vector>
#include <memory>

#include "common.h"
#include "vector3d.h"
#include "triangle.h"
#include "qbvh_accel.h"

#include "ray.h"
#include "brdf.h"
#include "envmap.h"
    
class SCENE_DLL Scene {
private:
    std::vector<Triangle>      _triangles;
    std::vector<int>           _bsdfIds;
    std::vector<BSDF>          _bsdfs;
    std::shared_ptr<QBVHAccel> _accel;
    Envmap _envmap;

public:
    Scene();
    ~Scene();

    template <class Ty>
    void add(const Ty& primitive, const BSDF& bsdf) {
        static_assert(std::is_base_of<IGeometry, Ty>::value, "Type inherits IGeometry can only be added to the scene.");

        std::vector<Triangle> newTriangles = primitive.triangulate();
        _triangles.insert(_triangles.end(), newTriangles.begin(), newTriangles.end());

        const int newBrdfId = (int)_bsdfs.size();
        const int numTriangles = (int)_bsdfIds.size();
        _bsdfIds.resize(_bsdfIds.size() + newTriangles.size());
        std::fill(_bsdfIds.begin() + numTriangles, _bsdfIds.end(), newBrdfId);
        _bsdfs.push_back(bsdf);
    }

    const Triangle& getTriangle(int triangleId) const;
    const BSDF& getBsdf(int triangleId) const;

    void clear();
    void setAccelerator();

    bool intersect(const Ray& ray, Intersection& isect) const;

    inline size_t numTriangles() const { return _triangles.size(); }
    inline const Envmap& envmap() const { return _envmap; }
    inline void setEnvmap(const Envmap& envmap) { _envmap = envmap; }

private:
    Scene(const Scene& scene) = delete;
    Scene& operator=(const Scene& scene) = delete;
};

#endif  // _SPICA_SCENE_H_
