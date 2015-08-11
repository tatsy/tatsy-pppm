#if defined(ENABLE_AVX) && (defined(_M_AMD64) || defined(_M_X64))
#define VECTOR_3D_EXPORT
#include "vector3d.h"

#include <cmath>
#include <sstream>

Vector3D::Vector3D()
    : xyz(0.0, 0.0, 0.0, 0.0) 
{
}

Vector3D::Vector3D(double x, double y, double z)
    : xyz(x, y, z, 0.0)
{
}

Vector3D::Vector3D(const Vector3D& v)
    : xyz(v.xyz)
{
}

Vector3D::~Vector3D()
{
}

double Vector3D::dot(const Vector3D& u, const Vector3D& v) {
    m256 prod;
    prod.d = _mm256_mul_pd(u.xyz.d, v.xyz.d);
    return prod.v[0] + prod.v[1] + prod.v[2];
}

Vector3D Vector3D::cross(const Vector3D& u, const Vector3D& v) {
    __m256d a_yzx = _mm256_setr_pd(u.xyz.v[1], u.xyz.v[2], u.xyz.v[0], 0.0);
    __m256d a_zxy = _mm256_setr_pd(u.xyz.v[2], u.xyz.v[0], u.xyz.v[1], 0.0);
    __m256d b_yzx = _mm256_setr_pd(v.xyz.v[1], v.xyz.v[2], v.xyz.v[0], 0.0);
    __m256d b_zxy = _mm256_setr_pd(v.xyz.v[2], v.xyz.v[0], v.xyz.v[1], 0.0);

    Vector3D ret;
    __m256d pos = _mm256_mul_pd(a_yzx, b_zxy);
    __m256d neg = _mm256_mul_pd(a_zxy, b_yzx);
    ret.xyz.d = _mm256_sub_pd(pos, neg);
    return ret;
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
    Vector3D ret;
    ret.xyz.d = _mm256_min_pd(u.xyz.d, v.xyz.d);
    return ret;
}

Vector3D Vector3D::maximum(const Vector3D& u, const Vector3D& v) {
    Vector3D ret;
    ret.xyz.d = _mm256_max_pd(u.xyz.d, v.xyz.d);
    return ret;
}

Vector3D Vector3D::sqrt(const Vector3D& v) {
    Vector3D ret;
    ret.xyz.d = _mm256_sqrt_pd(v.xyz.d);
    return ret;
}

Vector3D Vector3D::exp(const Vector3D& v) {
    Vector3D ret;
    ret.xyz.v[0] = ::exp(v.xyz.v[0]);
    ret.xyz.v[1] = ::exp(v.xyz.v[1]);
    ret.xyz.v[2] = ::exp(v.xyz.v[2]);
    return ret;
}

double Vector3D::x() const { return xyz.v[0]; }
double Vector3D::y() const { return xyz.v[1]; }
double Vector3D::z() const { return xyz.v[2]; }

void Vector3D::setX(double x) { xyz.v[0] = x; }
void Vector3D::setY(double y) { xyz.v[1] = y; }
void Vector3D::setZ(double z) { xyz.v[2] = z; }

double Vector3D::operator[](int d) const {
    Assertion(0 <= d && d <= 2, "Dimension index should be between 0 and 2!!");
    return xyz.v[d];
}

Vector3D& Vector3D::operator=(const Vector3D& v) {
    this->xyz = v.xyz;
    return *this;
}

Vector3D& Vector3D::operator+=(const Vector3D& v) {
    this->xyz.d = _mm256_add_pd(xyz.d, v.xyz.d);
    return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& v) {
    this->xyz.d = _mm256_sub_pd(xyz.d, v.xyz.d);
    return *this;
}

Vector3D Vector3D::operator-() const {
    Vector3D ret;
    __m256d minus = _mm256_setr_pd(-1.0, -1.0, -1.0, 0.0);
    ret.xyz.d = _mm256_mul_pd(xyz.d, minus);
    return ret;
}

Vector3D& Vector3D::operator*=(const Vector3D& v) {
    this->xyz.d = _mm256_mul_pd(xyz.d, v.xyz.d);
    return *this;
}

Vector3D& Vector3D::operator*=(double s) {
    float f = (float)s;
    __m256d fff = _mm256_setr_pd(s, s, s, 0.0);
    this->xyz.d = _mm256_mul_pd(xyz.d, fff);
    return *this;
}

Vector3D& Vector3D::operator/=(const Vector3D& v) {
    Assertion(v.x() != 0.0 && v.y() != 0.0 && v.z() != 0.0, "Zero division!!");
    this->xyz.d = _mm256_div_pd(xyz.d, v.xyz.d);
    return *this;
}

Vector3D& Vector3D::operator/=(double s) {
    Assertion(s != 0.0, "Zero division!!");
    this->operator*=(1.0 / s);
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
    Assertion(v.x() != 0.0 && v.y() != 0.0 && v.z() != 0.0, "Zero division!!");
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
