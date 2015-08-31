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

#include "common.h"
#include <string>
#include <immintrin.h>

// --------------------------------------------------
// ! 3D vector
// --------------------------------------------------
class VECTOR_3D_DLL Vector3D {
private:

#if defined(ENABLE_AVX) && (defined(_M_AMD64) || defined(_M_X64))
    union m256 {
        __m256d d;
        align_attrib(double,32) v[4];

        m256()
            : d(_mm256_setzero_pd())
        {
        }

        m256(double x, double y, double z, double w)
            : d(_mm256_setr_pd(x, y, z, w))
        {
        }
    };

    m256 xyz;
#else
    double _x, _y, _z;
#endif

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

    static Vector3D sqrt(const Vector3D& v);
    static Vector3D exp(const Vector3D& v);
    static Vector3D clamp(const Vector3D& v, 
                          const Vector3D& lo = Vector3D(0.0, 0.0, 0.0),
                          const Vector3D& hi = Vector3D(INFTY, INFTY, INFTY));

    inline double x() const { return _x; }
    inline double y() const { return _y; }
    inline double z() const { return _z; }

    inline void setX(double x) { _x = x; }
    inline void setY(double y) { _y = y; }
    inline void setZ(double z) { _z = z; }

    inline double operator[](int d) const {
        Assertion(0 <= d && d <= 2, "Dimension index should be between 0 and 2!!");
        switch (d) {
        case 0: return _x;
        case 1: return _y;
        case 2: return _z;
        }
        return 0;
    }

    std::string toString() const;
};

VECTOR_3D_DLL Vector3D operator+(const Vector3D& u, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator-(const Vector3D& u, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator*(const Vector3D& u, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator*(const Vector3D& v, double s);
VECTOR_3D_DLL Vector3D operator*(double s, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator/(const Vector3D& u, const Vector3D& v);
VECTOR_3D_DLL Vector3D operator/(const Vector3D& v, double s);

inline double luminance(const Vector3D& v) {
    return Vector3D::dot(v, Vector3D(0.2126, 0.7152, 0.0722));
}

#endif  // _VECTOR_3D_H_
