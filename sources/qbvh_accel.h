#ifndef _QBVH_ACCEL_H_
#define _QBVH_ACCEL_H_

#include <cstdlib>
#include <map>
#include <vector>
#include <xmmintrin.h>

#include "triangle.h"

typedef std::pair<Triangle, int> TriangleWithID;

class QBVHAccel {
private:

    struct QBVHNode {
        __m128 childBoxes[2][3];    // [min-max][x-y-z]
        QBVHNode* children[4];      // Child nodes
        std::vector<TriangleWithID> triangles;        
        char sepAxes[3];          // top-left-right
        bool isLeaf;

        QBVHNode()
            : childBoxes()
            , children()
            , triangles()
            , sepAxes()
            , isLeaf(false)
        {
        }

        ~QBVHNode()
        {
        }
    };

    static const int _maxNodeSize = 3;
    QBVHNode* _root;

public:
    QBVHAccel();
    QBVHAccel(const QBVHAccel& qbvh);
    QBVHAccel(QBVHAccel&& qbvh);
    ~QBVHAccel();

    QBVHAccel& operator=(const QBVHAccel& qbvh);
    QBVHAccel& operator=(QBVHAccel&& qbvh);

    void construct(const std::vector<Triangle>& triangles);

    // Intersection test
    // If ray is intersected, then return the index of the triangle.
    // If not, then return -1.
    int intersect(const Ray& ray, Hitpoint* hitpoint) const;

private:
    void release();
    void deleteNode(QBVHNode* node);
    QBVHNode* copyNode(QBVHNode* node);
        
    QBVHNode* constructRec(std::vector<TriangleWithID>& triangles, int dim);
};

#endif  // _SPICA_QBVH_ACCEL_H_