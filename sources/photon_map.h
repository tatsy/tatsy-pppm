#ifndef _PHOTON_MAP_H_
#define _PHOTON_MAP_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef PHOTON_MAP_EXPORT
        #define PHOTON_MAP_DLL __declspec(dllexport)
    #else
        #define PHOTON_MAP_DLL __declspec(dllimport)
    #endif
#else
    #define PHOTON_MAP_DLL
#endif

#include "photon.h"
#include "kdtree.h"
#include "readonly_interface.h"

#include "random.h"

class Scene;

class PHOTON_MAP_DLL PhotonMap : private IReadOnly {
private:
    KdTree<Photon> _kdtree;

public:
    PhotonMap();
    ~PhotonMap();

    void clear();
    void construct(const std::vector<Photon>& photons);

    void findKNN(const Photon& photon, std::vector<Photon>* photons, const int numTargetPhotons, const double targetRadius) const;
};

#endif
