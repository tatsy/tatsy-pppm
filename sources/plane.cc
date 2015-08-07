#define PLANE_EXPORT
#include "plane.h"
#include "common.h"
#include "triangle.h"

#include <iostream>

Plane::Plane()
    : _distance(0.0)
    , _normal()
{
}

Plane::Plane(double distance, const Vector3D& normal)
    : _distance(distance)
    , _normal(normal)
{
}

Plane::Plane(const Plane& plane) 
    : _distance(plane._distance)
    , _normal(plane._normal)
{
}

Plane::~Plane() {
}

Plane& Plane::operator=(const Plane& plane) {
    this->_distance = plane._distance;
    this->_normal   = plane._normal;
    return *this;
}

bool Plane::intersect(const Ray& ray, Hitpoint* hitpoint) const {
    double dt = Vector3D::dot(ray.direction(), _normal);
    if (dt > -EPS) {
        return false;
    }

    double dist = - _distance / dt;
    hitpoint->setDistance(dist);
    hitpoint->setPosition(ray.origin() + dist * ray.direction());
    hitpoint->setNormal(_normal);
    return true;
}

double Plane::area() const {
    std::cerr << "Area of Plane geometry is INFTY. It can make unexpected results." << std::endl;
    std::abort();
    return INFTY;
}

std::vector<Triangle> Plane::triangulate() const {
    std::cerr << "[ERROR] Plane cannot be triangulate!!" << std::endl;
    std::abort();
    return std::vector<Triangle>();
}

IGeometry* Plane::clone() const {
    return new Plane(*this);
}
