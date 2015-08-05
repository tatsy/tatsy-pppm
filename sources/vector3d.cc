#include "vector3d.h"

Vector3D::Vector3D()
    : xyz() 
{
    xyz.d = _mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f);
}

Vector3D::Vector3D(double x, double y, double z)
    : xyz()
{
    xyz.d = _mm_setr_ps((float)x, (float)y, (float)z, 0.0f);
}

Vector3D::Vector3D(const Vector3D& v)
    : xyz(v.xyz)
{
}

Vector3D::~Vector3D()
{
}

double Vector3D::dot(const Vector3D& u, const Vector3D& v) {
    m128 prod;
    prod.d = _mm_mul_ps(u.xyz.d, v.xyz.d);
    return prod.v[0] + prod.v[1] + prod.v[2];
}

Vector3D Vector3D::cross(const Vector3D& u, const Vector3D& v) {
    __m128 a_yzx = _mm_setr_ps(u.xyz.v[1], u.xyz.v[2], u.xyz.v[0], 0.0f);
    __m128 a_zxy = _mm_setr_ps(u.xyz.v[2], u.xyz.v[0], u.xyz.v[1], 0.0f);
    __m128 b_yzx = _mm_setr_ps(v.xyz.v[1], v.xyz.v[2], v.xyz.v[0], 0.0f);
    __m128 b_zxy = _mm_setr_ps(v.xyz.v[2], v.xyz.v[0], v.xyz.v[1], 0.0f);

    Vector3D ret;
    __m128 pos = _mm_mul_ps(a_yzx, b_zxy);
    __m128 neg = _mm_mul_ps(a_zxy, b_yzx);
    ret.xyz.d = _mm_sub_ps(pos, neg);
    return ret;
}

double Vector3D::norm() const {
    return sqrt(squaredNorm());
}

double Vector3D::squaredNorm() const {
    return Vector3D::dot(*this, *this);
}

Vector3D Vector3D::normalized() const {
    return *this / norm();
}

Vector3D Vector3D::reflect(const Vector3D& v, const Vector3D& n) {
    return v - n * (2.0 * Vector3D::dot(n, v));
}

Vector3D Vector3D::minimum(const Vector3D& u, const Vector3D& v) {
    Vector3D ret;
    ret.xyz.d = _mm_min_ps(u.xyz.d, v.xyz.d);
    return ret;
}

Vector3D Vector3D::maximum(const Vector3D& u, const Vector3D& v) {
    Vector3D ret;
    ret.xyz.d = _mm_max_ps(u.xyz.d, v.xyz.d);
    return ret;
}

Vector3D& Vector3D::operator=(const Vector3D& v) {
    this->xyz = v.xyz;
    return *this;
}

Vector3D& Vector3D::operator+=(const Vector3D& v) {
    this->xyz.d = _mm_add_ps(xyz.d, v.xyz.d);
    return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& v) {
    this->xyz.d = _mm_sub_ps(xyz.d, v.xyz.d);
    return *this;
}

Vector3D Vector3D::operator-() const {
    Vector3D ret;
    __m128 minus = _mm_setr_ps(-1.0f, -1.0f, -1.0f, 0.0f);
    ret.xyz.d = _mm_mul_ps(xyz.d, minus);
    return ret;
}

Vector3D& Vector3D::operator*=(double s) {
    float f = (float)s;
    __m128 fff = _mm_setr_ps(s, s, s, 0.0f);
    this->xyz.d = _mm_mul_ps(xyz.d, fff);
    return *this;
}

Vector3D& Vector3D::operator/=(double s) {
    assert(s != 0.0 && "Zero division!!");
    this->operator*=(1.0 / s);
    return *this;
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

Vector3D operator/(const Vector3D& v, double s) {
    Vector3D ret = v;
    ret /= s;
    return ret;
}
