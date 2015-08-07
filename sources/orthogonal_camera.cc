#define ORTHOGONAL_CAMERA_EXPORT
#include "orthogonal_camera.h"

#include <algorithm>

OrthogonalCamera::OrthogonalCamera()
    : ICamera()
    , _viewsize()
{
}

OrthogonalCamera::OrthogonalCamera(const Vector3D& center, const Vector3D& direction, const Vector3D& up, double width, double height, int imageWidth, int imageHeight, double sensitivity)
    : ICamera(center, direction, up, imageWidth, imageHeight, sensitivity)
    , _viewsize(width, height)
{
}

OrthogonalCamera::OrthogonalCamera(const OrthogonalCamera& camera)
    : ICamera(camera)
    , _viewsize(camera._viewsize)
{
}

OrthogonalCamera::~OrthogonalCamera()
{
}

OrthogonalCamera& OrthogonalCamera::operator=(const OrthogonalCamera& camera) {
    ICamera::operator=(camera);
    this->_viewsize = camera._viewsize;
    return *this;
}

Ray OrthogonalCamera::getRay(double pixelX, double pixelY) const {
    const double inv_scale = 1.0 / std::max(_imagesize.width(), _imagesize.height());
    Vector3D origin = _center + _viewsize.width()  * (2.0 * pixelX * inv_scale - 1.0) * _unitU
                              + _viewsize.height() * (2.0 * pixelY * inv_scale - 1.0) * _unitV;
    return Ray(origin, _direction);
}


