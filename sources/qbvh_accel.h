#ifndef _QBVH_ACCEL_H_
#define _QBVH_ACCEL_H_

#include <cstdlib>
#include <vector>
#include <xmmintrin.h>

#include "triangle.h"

class QBVHAccel {
private:
    struct QBVHNode {
        __m128 childBoxes[2][3];  // [min-max][x-y-z]
        QBVHNode* children[4];    // Child nodes
        Triangle* triangles;
        int  numTriangles;
        char sepAxes[3];          // top-left-right
        bool isLeaf;

        QBVHNode()
            : childBoxes()
            , children()
            , triangles(NULL)
            , numTriangles(0)
            , sepAxes()
            , isLeaf(false)
        {
        }

        ~QBVHNode()
        {
            delete[] triangles;
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

    bool intersect(const Ray& ray, Hitpoint* hitpoint) const;

private:
    void release();
    void deleteNode(QBVHNode* node);
    QBVHNode* copyNode(QBVHNode* node);
        
    QBVHNode* constructRec(std::vector<Triangle>& triangles, int dim);
};

#endif  // _SPICA_QBVH_ACCEL_H_

