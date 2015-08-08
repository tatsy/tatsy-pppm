#ifndef _REFLECTANCE_H_
#define _REFLECTANCE_H_

#include "common.h"
#include "vector3d.h"

inline bool checkTotalReflection(bool into, const Vector3D& in, const Vector3D& normal, const Vector3D& onormal, Vector3D* reflectdir, Vector3D* refractdir, double* fresnelRe, double* fresnelTr) {
    *reflectdir = Vector3D::reflect(in, normal);

    // Snell's rule
    const double nnt = into ? IOR_VACCUM / IOR_OBJECT : IOR_OBJECT / IOR_VACCUM;
    const double ddn = Vector3D::dot(in, onormal);
    const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);

    if (cos2t < 0.0) {
        // Total reflect
        *refractdir = Vector3D();
        *fresnelRe  = 1.0;
        *fresnelTr  = 0.0;
        return true;
    }

    *refractdir = (in * nnt - normal * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t))).normalized();

    static const double a = IOR_OBJECT - IOR_VACCUM;
    static const double b = IOR_OBJECT + IOR_VACCUM;
    static const double R0 = (a * a) / (b * b);

    const double c = 1.0 - (into ? -ddn : Vector3D::dot(*refractdir, -onormal));
    *fresnelRe = R0 + (1.0 - R0) * (c * c * c * c * c);
    *fresnelTr = 1.0 - (*fresnelRe);

    return false;
}

#endif  // _REFLECTANCE_H_
