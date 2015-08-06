#ifndef _SPICA_TEST_MACROS_H_
#define _SPICA_TEST_MACROS_H_

#include <algorithm>

#include <gtest/gtest.h>

#include "../sources/renderer.h"

inline void EXPECT_EQ_VEC(const Vector3D& expected, const Vector3D& actual, double tol = 1.0e-2) {
    if (std::abs(expected.x() - actual.x()) > tol ||
        std::abs(expected.y() - actual.y()) > tol ||
        std::abs(expected.z() - actual.z()) > tol) {
        FAIL() << "expected: " << expected.toString() << std::endl
               << "  actual: " << actual.toString()   << std::endl;
    }
    SUCCEED();
}

#endif  // _SPICA_TEST_MACROS_H_
