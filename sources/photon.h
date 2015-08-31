#ifndef _PHOTON_H_
#define _PHOTON_H_

#include "vector3d.h"

class Photon : public Vector3D {
private:
    Vector3D _flux;
    Vector3D _direction;
    Vector3D _normal;

public:
    Photon()
        : Vector3D()
        , _flux()
        , _direction()
        , _normal()
    {
    }

    Photon(const Vector3D& position, const Vector3D& flux, const Vector3D& direction, const Vector3D& normal)
        : Vector3D(position)
        , _flux(flux)
        , _direction(direction)
        , _normal(normal)
    {
    }

    Photon(const Photon& photon)
        : Vector3D(photon)
        , _flux(photon._flux)
        , _direction(photon._direction)
        , _normal(photon._normal)
    {
    }

    ~Photon()
    {
    }

    Photon& operator=(const Photon& photon) {
        Vector3D::operator=(photon);
        this->_flux = photon._flux;
        this->_direction = photon._direction;
        this->_normal = photon._normal;
        return *this;
    }

    inline Vector3D flux() const { return _flux; }
    inline Vector3D direction() const { return _direction; }
    inline Vector3D normal() const { return _normal; }
};

#endif  // _PHOTON_H_
