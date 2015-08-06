#include "gtest/gtest.h"

#include <algorithm>

#include "../sources/renderer.h"

TEST(Vector3DTest, InstanceTest) {
    Vector3D v;
    EXPECT_FLOAT_EQ(0.0, v.x());
    EXPECT_FLOAT_EQ(0.0, v.y());
    EXPECT_FLOAT_EQ(0.0, v.z());

    Vector3D u(1.0, 2.0, 3.0);
    EXPECT_FLOAT_EQ(1.0, u.x());
    EXPECT_FLOAT_EQ(2.0, u.y());
    EXPECT_FLOAT_EQ(3.0, u.z());

    v = Vector3D(u);
    EXPECT_FLOAT_EQ(1.0, v.x());
    EXPECT_FLOAT_EQ(2.0, v.y());
    EXPECT_FLOAT_EQ(3.0, v.z());

    v = Vector3D(2.0, 4.0, 6.0);
    EXPECT_FLOAT_EQ(2.0, v.x());
    EXPECT_FLOAT_EQ(4.0, v.y());
    EXPECT_FLOAT_EQ(6.0, v.z());
}

TEST(Vector3DTest, AlgebraTest) {
    Vector3D w;
    Vector3D v(1.0, 2.0, 3.0);
    Vector3D u(1.0, 2.0, 3.0);

    w = u + v;
    EXPECT_FLOAT_EQ(2.0, w.x());
    EXPECT_FLOAT_EQ(4.0, w.y());
    EXPECT_FLOAT_EQ(6.0, w.z());

    w = u - v;
    EXPECT_FLOAT_EQ(0.0, w.x());
    EXPECT_FLOAT_EQ(0.0, w.y());
    EXPECT_FLOAT_EQ(0.0, w.z());

    w = 5.0 * u * 5.0;
    EXPECT_FLOAT_EQ(25.0, w.x());
    EXPECT_FLOAT_EQ(50.0, w.y());
    EXPECT_FLOAT_EQ(75.0, w.z());

    w = u / 2.0;
    EXPECT_FLOAT_EQ(0.5, w.x());
    EXPECT_FLOAT_EQ(1.0, w.y());
    EXPECT_FLOAT_EQ(1.5, w.z());

    // ASSERT_DEATH(u /= 0.0, "");

    w = u * v;
    EXPECT_FLOAT_EQ(1.0, w.x());
    EXPECT_FLOAT_EQ(4.0, w.y());
    EXPECT_FLOAT_EQ(9.0, w.z());

    double nrm = u.norm();
    EXPECT_FLOAT_EQ(sqrt(14.0), nrm);

    w = u.normalized();
    EXPECT_FLOAT_EQ(u.x() / nrm, w.x());
    EXPECT_FLOAT_EQ(u.y() / nrm, w.y());
    EXPECT_FLOAT_EQ(u.z() / nrm, w.z());
}

TEST(Vector3DTest, DotCrossTest) {
    Vector3D u(1.0, 2.0, 3.0);
    Vector3D v(4.0, 5.0, 6.0);
    double dt = Vector3D::dot(u, v);
    EXPECT_FLOAT_EQ(32.0, dt);
    EXPECT_FLOAT_EQ(dt, Vector3D::dot(v, u));

    Vector3D w = Vector3D::cross(u, v);
    EXPECT_FLOAT_EQ(-3.0, w.x());
    EXPECT_FLOAT_EQ(6.0, w.y());
    EXPECT_FLOAT_EQ(-3.0, w.z());
}

TEST(Vector3DTest, MaxMinTest) {
    static const int nTrial = 100;
    Random rng = Random();

    Vector3D minv(INFTY, INFTY, INFTY);
    double minx = INFTY;
    double miny = INFTY;
    double minz = INFTY;

    for (int i = 0; i < nTrial; i++) {
        double x = rng.nextReal();
        double y = rng.nextReal();
        double z = rng.nextReal();
        minv = Vector3D::minimum(minv, Vector3D(x, y, z));
        minx = std::min(minx, x);
        miny = std::min(miny, y);
        minz = std::min(minz, z);

        EXPECT_FLOAT_EQ(minx, minv.x());
        EXPECT_FLOAT_EQ(miny, minv.y());
        EXPECT_FLOAT_EQ(minz, minv.z());
    }

    Vector3D maxv(-INFTY, -INFTY, -INFTY);
    double maxx = -INFTY;
    double maxy = -INFTY;
    double maxz = -INFTY;

    for (int i = 0; i < nTrial; i++) {
        double x = rng.nextReal();
        double y = rng.nextReal();
        double z = rng.nextReal();
        maxv = Vector3D::maximum(maxv, Vector3D(x, y, z));
        maxx = std::max(maxx, x);
        maxy = std::max(maxy, y);
        maxz = std::max(maxz, z);

        EXPECT_FLOAT_EQ(maxx, maxv.x());
        EXPECT_FLOAT_EQ(maxy, maxv.y());
        EXPECT_FLOAT_EQ(maxz, maxv.z());
    }
}
