#ifndef _COMMON_H_
#define _COMMON_H_

#include <cmath>
#include <iostream>

#include "directories.h"
#include "vector3d.h"

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
static const double INFTY = 1.0e20;
static const double EPS   = 1.0e-8;
static const double PI    = 4.0 * atan(1.0);
static const double invPI = 1.0 / PI;
static const double IOR_OBJECT = 1.5;
static const double IOR_VACCUM = 1.0;
static const double REFLECT_PROBABILITY = 0.5;

// ----------------------------------------------------------------------------
// Assertion
// ----------------------------------------------------------------------------

#ifndef NDEBUG
#define Assertion(PREDICATE, MSG) \
do { \
    if (!(PREDICATE)) { \
        std::cerr << "Asssertion \"" << #PREDICATE << "\" failed in " << __FILE__ \
        << " line " << __LINE__ << " : " << MSG << std::endl; \
        abort(); \
    } \
} while (false)
#else  // NDEBUG
#define Assertion(PREDICATE, MSG) do {} while (false)
#endif  // NDEBUG


// ----------------------------------------------------------------------------
// Special functions
// ----------------------------------------------------------------------------

extern void* enabler;
template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type *& = enabler>
inline T clamp(T v, T lo, T hi) {
    if (v < lo) v = lo;
    if (v > hi) v = hi;
    return v;
}

inline double luminance(const Vector3D& v) {
    return Vector3D::dot(v, Vector3D(0.2126, 0.7152, 0.0722));
}

// ----------------------------------------------------------------------------
// Parallel for
// ----------------------------------------------------------------------------
#ifdef _OPENMP
    #include <omp.h>
    #if defined(_WIN32) || defined(__WIN32__)
        #define ompfor __pragma(omp parallel for) for
        #define omplock __pragma(omp critical)
    #else
        #define ompfor _Pragma("omp parallel for") for
        #define omplock _Pragma("omp critical")
    #endif
    const int OMP_NUM_CORE = omp_get_max_threads();
    inline int omp_thread_id() { return omp_get_thread_num(); }
#else  // _OPENMP
    #define ompfor for
    #define omplock
    const int OMP_NUM_CORE = 1;
    inline int omp_thread_id() { return 0; }
#endif  // _OPENMP

// ----------------------------------------------------------------------------
// Alignment
// ----------------------------------------------------------------------------

#if defined(_WIN32) || defined(__WIN32__)
    #define align_attrib(typ, siz) __declspec(align(siz)) typ
#else
    #define align_attrib(typ, siz) typ __attribute__((aligned(siz)))
#endif

// ----------------------------------------------------------------------------
// isnan / isinf
// ----------------------------------------------------------------------------
#if defined(_WIN32) || defined(__WIN32__)
    #if _MSC_VER <= 1600
        #define isnan(x) _isnan(x)
        #define isinf(x) (!_finite(x))
    #endif
#endif

#endif  // _COMMON_H_
