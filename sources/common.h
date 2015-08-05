#ifndef _COMMON_H_
#define _COMMON_H_

#include <cmath>
#include <cassert>

#include "directories.h"

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
static const double INFTY = 1.0e20;
static const double EPS   = 1.0e-12;
static const double PI    = 4.0 * atan(1.0);

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
