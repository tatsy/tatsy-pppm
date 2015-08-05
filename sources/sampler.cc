#include "sampler.h"

#include <typeinfo>

#include "random_queue.h"
#include "hash_grid.h"

namespace sampler {

    namespace {

        Random rng = Random();

    }

    void onHemisphere(const Vector3D& normal, Vector3D* direction, double r1, double r2) {
        Vector3D u, v, w;
        w = normal;
        if (std::abs(w.x()) > EPS) {
            u = Vector3D::cross(Vector3D(0.0, 1.0, 0.0), w).normalized();
        } else {
            u = Vector3D::cross(Vector3D(1.0, 0.0, 0.0), w).normalized();
        }

        v = Vector3D::cross(w, u);

        const double t = 2.0 * PI * r1;
        const double z2 = r2;
        const double z2s = sqrt(z2);
        *direction = (u * cos(t) * z2s + v * sin(t) * z2s + w * sqrt(1.0 - z2)).normalized();        
    }



    void poissonDisk(const Trimesh& trimesh, const double minDist, std::vector<Vector3D>* points, std::vector<Vector3D>* normals) {
        // Sample random points on trimesh
        BBox bbox;
        std::vector<Vector3D> candPoints;
        std::vector<Vector3D> candNormals;
        for (int i = 0; i < trimesh.numFaces(); i++) {
            Triangle tri = trimesh.getTriangle(i);
            const double A = tri.area();
            const int nSample = static_cast<int>(std::ceil(4.0 * A / (minDist * minDist)));
            for (int k = 0; k < nSample; k++) {
                double u = rng.nextReal();
                double v = rng.nextReal();
                if (u + v >= 1.0) {
                    u = 1.0 - u;
                    v = 1.0 - v;
                }
                Vector3D p = tri.p0() + u * (tri.p1() - tri.p0()) + v * (tri.p2() - tri.p0());
                candPoints.push_back(p);
                candNormals.push_back(tri.normal());
                bbox.merge(p);
            }
        }

        // Create hash grid
        const int numCands = static_cast<int>(candPoints.size());
        Vector3D bsize = bbox.posMax() - bbox.posMin();
        const double scale = 1.0 / (2.0 * minDist);
        const int numPoints = candPoints.size();
        HashGrid<int> hashgrid;
        hashgrid.init(numPoints, scale, bbox);

        RandomQueue<int> que;
        for (int i = 0; i < numCands; i++) {
            que.push(i);
        }

        std::vector<int> sampledIDs;
        Vector3D marginv(2.0 * minDist, 2.0 * minDist, 2.0 * minDist);
        while (!que.empty()) {
            int id = que.pop();
            Vector3D v = candPoints[id];
            std::vector<int>& cellvs = hashgrid[v];

            bool accept = true;
            for (int k = 0; k < cellvs.size(); k++) {
                if ((candPoints[cellvs[k]] - v).squaredNorm() <= minDist * minDist) {
                    accept = false;
                    break;
                }
            }

            if (accept) {
                Vector3D boxMin = v - marginv;
                Vector3D boxMax = v + marginv;
                hashgrid.add(id, boxMin, boxMax);
                sampledIDs.push_back(id);
            }
        }

        // Store sampled points
        std::vector<int>::iterator it;
        for (it = sampledIDs.begin(); it != sampledIDs.end(); ++it) {
            points->push_back(candPoints[*it]);
            normals->push_back(candNormals[*it]);
        }
    }

}  // namespace sampler
