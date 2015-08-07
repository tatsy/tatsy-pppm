#define PHOTON_MAP_EXPORT
#include "photon_map.h"

#include "sampler.h"
#include "scene.h"

PhotonMap::PhotonMap()
    : _kdtree()
{
}

PhotonMap::~PhotonMap()
{
}

void PhotonMap::clear() {
    _kdtree.release();
}

void PhotonMap::construct(const std::vector<Photon>& photons) {
    _kdtree.construct(photons);
}

void PhotonMap::findKNN(const Photon& query, std::vector<Photon>* photons, const int numTargetPhotons, const double targetRadius) const {
    _kdtree.knnSearch(query, KnnQuery(K_NEAREST | EPSILON_BALL, numTargetPhotons, targetRadius), photons);
}
