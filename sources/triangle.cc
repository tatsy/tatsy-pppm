#define TRIANGLE_EXPORT
#include "triangle.h"
#include "common.h"

Triangle::Triangle()
    : _p0()
    , _p1()
    , _p2()
{
}

Triangle::Triangle(const Vector3D& p0, const Vector3D& p1, const Vector3D& p2)
    : _p0(p0)
    , _p1(p1)
    , _p2(p2)
{
}

Triangle::Triangle(const Triangle& tri)
    : _p0(tri._p0)
    , _p1(tri._p1)
    , _p2(tri._p2)
{
}

Triangle::~Triangle()
{
}

Triangle& Triangle::operator=(const Triangle& tri) {
    this->_p0 = tri._p0;
    this->_p1 = tri._p1;
    this->_p2 = tri._p2;
    return *this;
}

Vector3D Triangle::p(int id) const {
    assert(0 <= id && id <= 2 && "ID must be between 0 and 2");
    if (id == 0) return _p0;
    if (id == 1) return _p1;
    if (id == 2) return _p2;
    return Vector3D();
}

Vector3D Triangle::normal() const {
    return Vector3D::cross(_p1 - _p0, _p2 - _p0).normalized();
}

Vector3D Triangle::gravity() const {
    return (_p0 + _p1 + _p2) / 3.0;
}

bool Triangle::intersect(const Ray& ray, Hitpoint* hitpoint) const {
    Vector3D e1 = _p1 - _p0;
    Vector3D e2 = _p2 - _p0;
    Vector3D pVec = Vector3D::cross(ray.direction(), e2);
    double det = Vector3D::dot(e1, pVec);
    if (det > -EPS && det <EPS) return false;
    double invdet = 1.0 / det;
        
    Vector3D tVec = ray.origin() - _p0;
    double u = Vector3D::dot(tVec, pVec) * invdet;
    if (u < 0.0 || u > 1.0) return false;

    Vector3D qVec = Vector3D::cross(tVec, e1);
    double v = Vector3D::dot(ray.direction(), qVec) * invdet;
    if (v < 0.0 || u + v > 1.0) return false;

    double t = Vector3D::dot(e2, qVec) * invdet;
    if (t > EPS) {
        hitpoint->setDistance(t);
        hitpoint->setPosition(ray.origin() + t * ray.direction());
        hitpoint->setNormal(this->normal());
        return true;
    }

    return false;
}

double Triangle::area() const {
    Vector3D e1 = _p1 - _p0;
    Vector3D e2 = _p2 - _p1;
    return 0.5 * Vector3D::cross(e1, e2).norm();
}

IGeometry* Triangle::clone() const {
    return new Triangle(*this);
}
