#define ORTHOGONAL_CAMERA_EXPORT
#include "orthogonal_camera.h"

#include <algorithm>

OrthogonalCamera::OrthogonalCamera()
    : _center()
    , _direction()
    , _viewsize()
    , _imagesize()
    , _unitU()
    , _unitV()
{
}

OrthogonalCamera::OrthogonalCamera(const Vector3D& center, const Vector3D& direction, double width, double height, int imageWidth, int imageHeight)
    : _center(center)
    , _direction(direction)
    , _viewsize(width, height)
    , _imagesize(imageWidth, imageHeight)
    , _unitU()
    , _unitV()
{
    if (std::abs(direction.x()) > EPS) {
        _unitV = Vector3D::cross(Vector3D(0.0, 1.0, 0.0), direction);
    } else {
        _unitV = Vector3D::cross(Vector3D(1.0, 0.0, 0.0), direction);
    }
    _unitU = Vector3D::cross(direction, _unitV);
}

OrthogonalCamera::OrthogonalCamera(const OrthogonalCamera& camera)
    : _center(camera._center)
    , _direction(camera._direction)
    , _viewsize(camera._viewsize)
    , _imagesize(camera._imagesize)
    , _unitU(camera._unitU)
    , _unitV(camera._unitV)
{
}

OrthogonalCamera::~OrthogonalCamera()
{
}

OrthogonalCamera& OrthogonalCamera::operator=(const OrthogonalCamera& camera) {
    this->_center = camera._center;
    this->_direction = camera._direction;
    this->_viewsize = camera._viewsize;
    this->_imagesize = camera._imagesize;
    this->_unitU = camera._unitU;
    this->_unitV = camera._unitV;
    return *this;
}

Ray OrthogonalCamera::getRay(double pixelX, double pixelY) const {
    const double inv_scale = 1.0 / std::max(_imagesize.width(), _imagesize.height());
    Vector3D origin = _center + _viewsize.width()  * (pixelX * inv_scale - 0.5) * _unitU
                              + _viewsize.height() * (pixelY * inv_scale - 0.5) * _unitV;
    return Ray(origin, _direction);
}


