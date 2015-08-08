#if !(defined(ENABLE_SSE2) && (defined(_M_AMD64) || defined(_M_X64)))
#define VECTOR_3D_EXPORT
#include "vector3d.h"

#include <cassert>
#include <algorithm>
#include <sstream>

#include "common.h"

Vector3D::Vector3D()
    : _x(0.0)
    , _y(0.0)
    , _z(0.0)
{
}

Vector3D::Vector3D(double x, double y, double z)
    : _x(x)
    , _y(y)
    , _z(z)
{
}

Vector3D::Vector3D(const Vector3D& v)
    : _x(v._x)
    , _y(v._y)
    , _z(v._z)
{
}

Vector3D::~Vector3D()
{
}

double Vector3D::dot(const Vector3D& u, const Vector3D& v) {
    return u._x * v._x + u._y * v._y + u._z * v._z;
}

Vector3D Vector3D::cross(const Vector3D& u, const Vector3D& v) {
    double cx = u._y * v._z - u._z * v._y;
    double cy = u._z * v._x - u._x * v._z;
    double cz = u._x * v._y - u._y * v._x;
    return Vector3D(cx, cy, cz);
}

double Vector3D::norm() const {
    return ::sqrt(squaredNorm());
}

double Vector3D::squaredNorm() const {
    return Vector3D::dot(*this, *this);
}

Vector3D Vector3D::normalized() const {
    return *this / (norm() + EPS);
}

Vector3D Vector3D::reflect(const Vector3D& v, const Vector3D& n) {
    return v - n * (2.0 * Vector3D::dot(n, v));
}

Vector3D Vector3D::minimum(const Vector3D& u, const Vector3D& v) {
    const double rx = std::min(u._x, v._x);
    const double ry = std::min(u._y, v._y);
    const double rz = std::min(u._z, v._z);
    return Vector3D(rx, ry, rz);
}

Vector3D Vector3D::maximum(const Vector3D& u, const Vector3D& v) {
    const double rx = std::max(u._x, v._x);
    const double ry = std::max(u._y, v._y);
    const double rz = std::max(u._z, v._z);
    return Vector3D(rx, ry, rz);
}

Vector3D Vector3D::sqrt(const Vector3D& v) {
    return Vector3D(::sqrt(v._x), ::sqrt(v._y), ::sqrt(v._z));
}

Vector3D Vector3D::exp(const Vector3D& v) {
    return Vector3D(::exp(v._x), ::exp(v._y), ::exp(v._z));
}

double Vector3D::x() const { return _x; }
double Vector3D::y() const { return _y; }
double Vector3D::z() const { return _z; }

void Vector3D::setX(double x) { _x = x; }
void Vector3D::setY(double y) { _y = y; }
void Vector3D::setZ(double z) { _z = z; }

double Vector3D::operator[](int d) const {
    Assertion(0 <= d && d <= 2, "Dimension index should be between 0 and 2!!");
    switch ( d ) {
    case 0: return _x;
    case 1: return _y;
    case 2: return _z;
    }
    return 0;
}

Vector3D& Vector3D::operator=(const Vector3D& v) {
    this->_x = v._x;
    this->_y = v._y;
    this->_z = v._z;
    return *this;
}

Vector3D& Vector3D::operator+=(const Vector3D& v) {
    this->_x += v._x;
    this->_y += v._y;
    this->_z += v._z;
    return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& v) {
    this->_x -= v._x;
    this->_y -= v._y;
    this->_z -= v._z;
    return *this;
}

Vector3D Vector3D::operator-() const {
    return Vector3D(-_x, -_y, -_z);
}

Vector3D& Vector3D::operator*=(const Vector3D& v) {
    this->_x *= v._x;
    this->_y *= v._y;
    this->_z *= v._z;
    return *this;
}

Vector3D& Vector3D::operator*=(double s) {
    this->_x *= s;
    this->_y *= s;
    this->_z *= s;
    return *this;
}

Vector3D& Vector3D::operator/=(const Vector3D& v) {
    Assertion(v.x() != 0.0 && v.y() != 0.0 && v.z() != 0.0, "Zero division!!");
    this->_x /= v._x;
    this->_y /= v._y;
    this->_z /= v._z;
    return *this;
}

Vector3D& Vector3D::operator/=(double s) {
    Assertion(s != 0.0, "Zero division!!");
    double d = 1.0 / s;
    this->_x *= d;
    this->_y *= d;
    this->_z *= d;
    return *this;
}

std::string Vector3D::toString() const {
    std::stringstream ss;
    ss << "(" << x() << ", " << y() << ", " << z() << ")";
    return ss.str();
}

Vector3D operator+(const Vector3D& u, const Vector3D& v) {
    Vector3D ret = u;
    ret += v;
    return ret;
}

Vector3D operator-(const Vector3D& u, const Vector3D& v) {
    Vector3D ret = u;
    ret -= v;
    return ret;
}

Vector3D operator*(const Vector3D& u, const Vector3D& v) {
    Vector3D ret = u;
    ret *= v;
    return ret;
}

Vector3D operator*(const Vector3D& v, double s) {
    Vector3D ret = v;
    ret *= s;
    return ret;
}

Vector3D operator*(double s, const Vector3D& v) {
    Vector3D ret = v;
    ret *= s;
    return ret;
}

Vector3D operator/(const Vector3D& u, const Vector3D& v) {
    Vector3D ret = u;
    ret /= v;
    return ret;
}

Vector3D operator/(const Vector3D& v, double s) {
    Vector3D ret = v;
    ret /= s;
    return ret;
}

#endif
