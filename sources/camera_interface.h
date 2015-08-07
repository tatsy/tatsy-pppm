#ifndef _CAMERA_INTERFACE_H_
#define _CAMERA_INTERFACE_H_

#include "vector3d.h"
#include "size.h"
#include "ray.h"

class ICamera {
protected:
    Vector3D _center;
    Vector3D _direction;
    Size<int> _imagesize;
    double _sensitivity;

    Vector3D _unitU;
    Vector3D _unitV;

public:
    ICamera()
        : _center()
        , _direction()
        , _imagesize()
        , _sensitivity(0.0)
        , _unitU()
        , _unitV()
    {
    }

    ICamera(const Vector3D& center, const Vector3D& direction, const Vector3D& up, int imageWidth, int imageHeight, double sensitivity)
        : _center(center)
        , _direction(direction)
        , _imagesize(imageWidth, imageHeight)
        , _sensitivity(sensitivity)
        , _unitU()
        , _unitV()
    {
        _unitU = Vector3D::cross(direction, up);
        _unitV = Vector3D::cross(_unitU, direction);
    }

    ICamera(const ICamera& camera)
        : _center(camera._center)
        , _direction(camera._direction)
        , _imagesize(camera._imagesize)
        , _sensitivity(camera._sensitivity)
        , _unitU(camera._unitU)
        , _unitV(camera._unitV)
    {
    }

    virtual ~ICamera()
    {
    }

    ICamera& operator=(const ICamera& camera) {
        this->_center = camera._center;
        this->_direction = camera._direction;
        this->_imagesize = camera._imagesize;
        this->_sensitivity = camera._sensitivity;
        this->_unitU = camera._unitU;
        this->_unitV = camera._unitV;
        return *this;
    }

    virtual Ray getRay(double pixelX, double pixelY) const {
        return Ray();
    };

    inline const Vector3D& center() const { return _center; }
    inline const Vector3D& direction() const { return _direction; }
    inline const Size<int>& imagesize() const { return _imagesize; }
    inline double sensitivity() const { return _sensitivity; }
};

#endif  // _CAMERA_INTERFACE_H_
