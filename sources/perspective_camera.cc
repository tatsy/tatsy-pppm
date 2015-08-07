#define PERSPECTIVE_CAMERA_EXPORT
#include "perspective_camera.h"

#include <cmath>

PerspectiveCamera::PerspectiveCamera()
    : ICamera()
    , _fov(0.0)
    , _aspect(0.0)
    , _halfTangent(0.0)
{
}

PerspectiveCamera::PerspectiveCamera(const Vector3D& center, const Vector3D& direction, const Vector3D& up, double fov, int imageWidth, int imageHeight, double sensitivity)
    : ICamera(center, direction, up, imageWidth, imageHeight, sensitivity)
    , _fov(fov)
    , _aspect((double)imageWidth / (double)imageHeight)
    , _halfTangent(tan(fov * 0.5))
{
}

PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera& camera)
    : ICamera(camera)
    , _fov(camera._fov)
    , _halfTangent(camera._halfTangent)
{
}

PerspectiveCamera::~PerspectiveCamera()
{
}

PerspectiveCamera& PerspectiveCamera::operator=(const PerspectiveCamera& camera) {
    ICamera::operator=(camera);
    this->_fov = camera._fov;
    this->_halfTangent = camera._halfTangent;
    return *this;
}

Ray PerspectiveCamera::getRay(double pixelX, double pixelY) const {
    Vector3D dir = _direction + _halfTangent * _aspect * (2.0 * pixelX / _imagesize.width() - 1.0) * _unitU
                              + _halfTangent * (2.0 * pixelY / _imagesize.height() - 1.0) * _unitV;
   return Ray(_center, dir.normalized());
}