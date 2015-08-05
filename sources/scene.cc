#include "scene.h"

#include <cstring>
    
Scene::Scene()
    : _lightIDs()
    , _objects()
    , _brdfs()
{
}

Scene::~Scene()
{
    for (int i = 0; i < _objects.size(); i++) {
        delete _objects[i];
    }
}

const IGeometry* Scene::getObject(int id) const {
    assert(id >= 0 && id < _objects.size() && "Object index out of bounds");
    return _objects[id];
}

const BRDF& Scene::getBrdf(int id) const {
    assert(id >= 0 && id < _brdfs.size() && "Object index out of boudns");
    return _brdfs[id];
}

void Scene::clear() {
    _lightIDs.clear();
    for (int i = 0; i < _objects.size(); i++) {
        delete _objects[i];
    }
    _objects.clear();
    _brdfs.clear();
}

bool Scene::intersect(const Ray& ray, Intersection& isect) const {
    // Linear search
    int objID = -1;
    Hitpoint hitpoint;
    for (int i = 0; i < _objects.size(); i++) {
        Hitpoint hpTemp;
        if (_objects[i]->intersect(ray, &hpTemp)) {
            if (hitpoint.distance() > hpTemp.distance()) {
                objID = i;
                hitpoint = hpTemp;
            }
        }
    }

    isect.setObjectId(objID);
    isect.setHitpoint(hitpoint);
    return objID != -1;
}
