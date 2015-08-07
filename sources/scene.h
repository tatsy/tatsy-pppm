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
    std::vector<int>           _brdfIds;
    std::vector<BRDF>          _brdfs;
    std::shared_ptr<QBVHAccel> _accel;
    Envmap _envmap;

public:
    Scene();
    ~Scene();

    template <class Ty>
    void add(const Ty& primitive, const BRDF& brdf) {
        static_assert(std::is_base_of<IGeometry, Ty>::value, "Type inherits IGeometry can only be added to the scene.");

        std::vector<Triangle> newTriangles = primitive.triangulate();
        _triangles.insert(_triangles.end(), newTriangles.begin(), newTriangles.end());

        const int newBrdfId = (int)_brdfs.size();
        const int numTriangles = (int)_brdfIds.size();
        _brdfIds.resize(_brdfIds.size() + newTriangles.size());
        std::fill(_brdfIds.begin() + numTriangles, _brdfIds.end(), newBrdfId);
        _brdfs.push_back(brdf);
    }

    const Triangle& getTriangle(int triangleId) const;
    const BRDF& getBrdf(int triangleId) const;

    void clear();
    void setAccelerator();

    bool intersect(const Ray& ray, Intersection& isect) const;

    inline const Envmap& envmap() const { return _envmap; }
    inline void setEnvmap(const Envmap& envmap) { _envmap = envmap; }

private:
    Scene(const Scene& scene) = delete;
    Scene& operator=(const Scene& scene) = delete;
};

#endif  // _SPICA_SCENE_H_
