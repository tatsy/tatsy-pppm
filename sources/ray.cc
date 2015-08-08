#define RAY_EXPORT
#include "ray.h"
#include "common.h"

#include <cmath>

Ray::Ray()
    : _origin()
    , _direction()
    , _invdir()
{
    calcInvdir();
}

Ray::Ray(const Vector3D& origin, const Vector3D& direction)
    : _origin(origin)
    , _direction(direction)
    , _invdir()
{
    Assertion(std::abs(1.0 - direction.norm()) < 1.0e-3, "Direction must be unit vector");
    calcInvdir();
}

Ray::Ray(const Ray& ray)
    : _origin()
    , _direction()
    , _invdir()
{
    operator=(ray);
}

Ray::~Ray() {
}

Ray& Ray::operator=(const Ray& ray) {
    this->_origin = ray._origin;
    this->_direction = ray._direction;
    this->_invdir = ray._invdir;
    return *this;
}

void Ray::calcInvdir() {
    _invdir.setX((_direction.x() == 0.0) ? INFTY : 1.0 / _direction.x());
    _invdir.setY((_direction.y() == 0.0) ? INFTY : 1.0 / _direction.y());
    _invdir.setZ((_direction.z() == 0.0) ? INFTY : 1.0 / _direction.z());
}

Hitpoint::Hitpoint()
    : _distance(INFTY)
    , _normal()
    , _position() {
}

Hitpoint::Hitpoint(const Hitpoint& hp)
    : _distance(hp._distance)
    , _normal(hp._normal)
    , _position(hp._position) {
}

Hitpoint::~Hitpoint() {
}

Hitpoint& Hitpoint::operator=(const Hitpoint& hp) {
    this->_distance = hp._distance;
    this->_normal = hp._normal;
    this->_position = hp._position;
    return *this;
}

Intersection::Intersection()
    : _hitPoint()
    , _objectId(-1) {
}

Intersection::Intersection(const Intersection& intersection)
    : _hitPoint(intersection._hitPoint)
    , _objectId(intersection._objectId) {
}

Intersection::~Intersection() {
}

Intersection& Intersection::operator=(const Intersection& intersection) {
    this->_hitPoint = intersection._hitPoint;
    this->_objectId = intersection._objectId;
    return *this;
}

double Intersection::hittingDistance() const {
    return _hitPoint.distance();
}
