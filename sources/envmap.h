#ifndef _ENVMAP_H_
#define _ENVMAP_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef ENVMAP_EXPORT
        #define ENVMAP_DLL __declspec(dllexport)
    #else
        #define ENVMAP_DLL __declspec(dllimport)
    #endif
#else
    #define ENVMAP_DLL
#endif

#include <vector>

#include "vector3d.h"
#include "photon.h"

#include "image.h"
#include "random.h"
#include "random_sequence.h"

class Photon;

class ENVMAP_DLL Envmap {
private:
    Image _image;
    Image _importance;
    std::vector<double> _pdf;
    std::vector<double> _cdf;
    static const int IMPORTANCE_MAP_SIZE = 64;

public:
    Envmap();
    Envmap(const std::string& filename);

    void resize(int width, int height);
    void clearColor(const Vector3D& color);

    Vector3D sampleFromDir(const Vector3D& dir) const;
    Photon samplePhoton(RandomSequence& rseq, const int numPhotons) const;
    const Image& getImage() const;

private:
    void createImportanceMap();
};

#endif  // _ENVMAP_H_
