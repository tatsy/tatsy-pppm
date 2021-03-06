#define ENVMAP_EXPORT
#include "envmap.h"

#include <cstdlib>
#include <cstring>
#include <algorithm>

#include "common.h"

namespace {
    void directionToPolarCoord(const Vector3D& dir, double* theta, double* phi) {
        *theta = acos(dir.y());
        *phi = atan2(dir.z(), dir.x());
        if (*phi < 0.0) {
            *phi += 2.0 * PI;
        }
    }
}

Envmap::Envmap()
    : _image()
    , _importance()
    , _pdf()
    , _cdf()
{
}

Envmap::Envmap(const std::string& filename)
    : _image()
    , _importance()
    , _pdf()
    , _cdf()
{
    _image.load(filename);
    createImportanceMap();
}

void Envmap::resize(int width, int height) {
    _image.resize(width, height);
    _importance.resize(width, height);
}

void Envmap::clearColor(const Vector3D& color) {
    _image.fill(color);
}

Vector3D Envmap::sampleFromDir(const Vector3D& dir) const {
    double theta, phi;
    directionToPolarCoord(dir, &theta, &phi);
        
    const double iblv = theta / PI;
    const double iblu = phi / (2.0 * PI);

    const double iblx = iblu * _image.width();
    const double ibly = iblv * _image.height();
    
    const int ix = clamp((int)iblx, 0, (int)_image.width() - 1);
    const int iy = clamp((int)ibly, 0, (int)_image.height() - 1);
    return _image(ix, iy);
}

Photon Envmap::samplePhoton(RandomSequence& rseq, const int numPhotons) const {
    const double R = 50.0;
    const int width  = IMPORTANCE_MAP_SIZE;
    const int height = IMPORTANCE_MAP_SIZE;

    const double r = rseq.pop();
    std::vector<double>::const_iterator it = std::lower_bound(_cdf.begin(), _cdf.end(), r);
    const int index = it - _cdf.begin();

    // Direction
    const int ix = index % width;
    const int iy = index / width;
    const double u = (ix + rseq.pop()) / width;
    const double v = (iy + rseq.pop()) / height;

    const double phi = u * PI * 2.0;
    
    const double   y    = (1.0 - v) * 2.0 - 1.0;
    const Vector3D dir  = Vector3D(sqrt(1.0 - y * y) * cos(phi), y, sqrt(1.0 - y * y) * sin(phi));
    const double   area = (_image.width() * _image.height()) / (4.0 * PI * width * height);
    const double   pdf  = _pdf[index];
    const Vector3D currentFlux = sampleFromDir(dir) * (area / (pdf * numPhotons));

    return Photon(R * dir, currentFlux, -dir, -dir);                
}

void Envmap::createImportanceMap() {
    _importance.resize(IMPORTANCE_MAP_SIZE, IMPORTANCE_MAP_SIZE);

    const int width = _importance.width();
    const int height = _importance.height();

    double total = 0.0;
    for (int iy = 0; iy < height; iy++) {
        for (int ix = 0; ix < width; ix++) {
            const double u = static_cast<double>(ix) / width;
            const double v = static_cast<double>(iy) / height;
            const double next_u = static_cast<double>(ix + 1) / width;
            const double next_v = static_cast<double>(iy + 1) / height;

            const int begin_x = static_cast<int>(u * _image.width());
            const int end_x = clamp<int>(static_cast<int>(next_u * _image.width()), 0, _image.width());
            const int begin_y = static_cast<int>(v * _image.height());
            const int end_y = clamp<int>(static_cast<int>(next_v * _image.height()), 0, _image.height());

            Vector3D accum;
            const int area = (end_y - begin_y) * (end_x - begin_x);
            for (int ty = begin_y; ty < end_y; ty++) {
                for (int tx = begin_x; tx < end_x; tx++) {
                    accum += _image(tx, ty);
                }
            }

            const double lum = luminance(accum);
            _importance.pixel(ix, iy) = Vector3D(1.0, 1.0, 1.0) * lum / area;
            total += _importance(ix, iy).x();
        }
    }

    // Normalization
    for (int iy = 0; iy < height; iy++) {
        for (int ix = 0; ix < width; ix++) {
            _importance.pixel(ix, iy) = _importance(ix, iy) / (total + EPS);
        }
    }

    // Warped importance map
    total = 0.0;
    _pdf.resize(width * height);
    const int superX = _image.width() / width;
    const int superY = _image.height() / height;
    for (int iy = 0; iy < height; iy++) {
        for (int ix = 0; ix < width; ix++) {
            Vector3D accum;
            for (int sy = 0; sy < superY; sy++) {
                for (int sx = 0; sx < superX; sx++) {
                    const double u = (ix + static_cast<double>(sx) / superX) / width;
                    const double v = (iy + static_cast<double>(sy) / superY) / height;

                    const double phi = u * 2.0 * PI;
                    const double y = (1.0 - v) * 2.0 - 1.0;

                    const Vector3D dir = Vector3D(sqrt(1.0 - y * y) * cos(phi), y, sqrt(1.0 - y * y) * sin(phi));
                    accum += sampleFromDir(dir) / (superX * superY);
                }
            }

            double l = luminance(accum);
            _pdf[iy * width + ix] = l * l;
            total += _pdf[iy * width + ix];
        }
    }

    // Normalization
    _cdf.resize(width * height);
    _pdf[0] /= (total + EPS);
    _cdf[0] = _pdf[0];
    for (int i = 1; i < width * height; i++) {
        _pdf[i] /= (total + EPS);
        _cdf[i] = _pdf[i] + _cdf[i - 1];
    }
}

const Image& Envmap::getImage() const {
    return _image;
}
