#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#include "vector3d.h"
#include "triangle.h"
#include "trimesh.h"
#include "random.h"

namespace sampler {

    void onHemisphere(const Vector3D& normal, Vector3D* direction, double r1, double r2);

    void poissonDisk(const std::vector<Triangle>& triangles, const double minDist, std::vector<Vector3D>* points, std::vector<Vector3D>* normals);

}  // namespace sampler

#endif  // _SAMPLER_H_
