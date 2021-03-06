#define SCENE_EXPORT
#include "scene.h"

#include <cstring>
    
Scene::Scene()
    : _triangles()
    , _bsdfs()
    , _envmap()
{
    _envmap.resize(512, 512);
    _envmap.clearColor(Vector3D(0.0, 0.0, 0.0));
}

Scene::~Scene()
{
}

const Triangle& Scene::getTriangle(int id) const {
    Assertion(id >= 0 && id < _triangles.size(), "Triangle index out of bounds");
    return _triangles[id];
}

const BSDF& Scene::getBsdf(int id) const {
    Assertion(id >= 0 && id < _bsdfIds.size(), "Object index out of boudns");
    return _bsdfs[_bsdfIds[id]];
}

void Scene::clear() {
    _triangles.clear();
    _bsdfIds.clear();
    _bsdfs.clear();
}

void Scene::setAccelerator() {
    _accel = std::shared_ptr<QBVHAccel>(new QBVHAccel());
    _accel->construct(_triangles);
}

bool Scene::intersect(const Ray& ray, Intersection& isect) const {
    Hitpoint hitpoint;
    int triID = _accel->intersect(ray, &hitpoint);

    isect.setObjectId(triID);
    isect.setHitpoint(hitpoint);
    return triID != -1;
}
