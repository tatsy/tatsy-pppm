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

#include "geometry_interface.h"

#include "common.h"
#include "vector3d.h"
#include "ray.h"
#include "brdf.h"
#include "envmap.h"
    
class SCENE_DLL Scene {
private:
    std::vector<int>        _lightIDs;
    std::vector<IGeometry*> _objects;
    std::vector<BRDF>       _brdfs;

    Envmap _envmap;

public:
    Scene();
    ~Scene();

    template <class Ty>
    void add(const Ty& primitive, const BRDF& brdf, bool isLight = false) {
        static_assert(std::is_base_of<IGeometry, Ty>::value, "Type inherits IGeometry can only be added to the scene.");
        
        if (isLight) {
            _lightIDs.push_back(_objects.size());
        }
        _objects.push_back(primitive.clone());
        _brdfs.push_back(brdf);
    }

    const IGeometry* getObject(int id) const;
    const BRDF& getBrdf(int id) const;

    void clear();

    bool intersect(const Ray& ray, Intersection& isect) const;

    inline int numObjects() const { return (int)_objects.size(); }
    inline const Envmap& envmap() const { return _envmap; }
    inline void setEnvmap(const Envmap& envmap) { _envmap = envmap; }

private:
    Scene(const Scene& scene) = delete;
    Scene& operator=(const Scene& scene) = delete;
};

#endif  // _SPICA_SCENE_H_
