#ifndef _VECTOR_3D_H_
#define _VECTOR_3D_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef VECTOR_3D_EXPORT
        #define VECTOR_3D_DLL __declspec(dllexport)
    #else
        #define VECTOR_3D_DLL __declspec(dllimport)
    #endif
#else
    #define VECTOR_3D_DLL
#endif

#include <string>
#include <xmmintrin.h>

#include "common.h"

class VECTOR_3D_DLL Vector3D {
private:

    union m128 {
        __m128 d;
        align_attrib(float,16) v[4];
    };

    m128 xyz;

public:
    Vector3D();
    Vector3D(double x, double y, double z);
    Vector3D(const Vector3D& v);
    ~Vector3D();

    Vector3D& operator=(const Vector3D& v);
    Vector3D& operator+=(const Vector3D& v);
    Vector3D& operator-=(const Vector3D& v);
    Vector3D operator-() const;

    Vector3D& operator*=(const Vector3D& v);
    Vector3D& operator*=(double s);
    Vector3D& operator/=(const Vector3D& v);
    Vector3D& operator/=(double s);

    static double dot(const Vector3D& u, const Vector3D& v);
    static Vector3D cross(const Vector3D& u, const Vector3D& v);

    double norm() const;
    double squaredNorm() const;
    Vector3D normalized() const;

    static Vector3D reflect(const Vector3D& v, const Vector3D& n);

    static Vector3D minimum(const Vector3D& u, const Vector3D& v);
    static Vector3D maximum(const Vector3D& u, const Vector3D& v);

    inline double x() const { return xyz.v[0]; }
    inline double y() const { return xyz.v[1]; }
    inline double z() const { return xyz.v[2]; }

    inline void setX(double x) { xyz.v[0] = (float)x; }
    inline void setY(double y) { xyz.v[1] = (float)y; }
    inline void setZ(double z) { xyz.v[2] = (float)z; }

    inline double operator[](int d) const { return xyz.v[d]; }

    std::string toString() const;
};

VECTOR_3D_DLL Vector3D operator+(const Vector3D& u, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator-(const Vector3D& u, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator*(const Vector3D& u, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator*(const Vector3D& v, double s);
VECTOR_3D_DLL Vector3D operator*(double s, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator/(const Vector3D& u, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator/(const Vector3D& v, double s);

#endif  // _VECTOR_3D_H_
