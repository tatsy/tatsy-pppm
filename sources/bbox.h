#ifndef _BBOX_H_
#define _BBOX_H_

#include "vector3d.h"
#include "triangle.h"
#include "ray.h"

// ----------------------------------------
// Axis-aligned bounding box
// ----------------------------------------    
class BBox {
private:
    Vector3D _posMin;    // Position of minimum corner
    Vector3D _posMax;    // Position of maximum corner
        
public:
    BBox();
    explicit BBox(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
    explicit BBox(const Vector3D& posMin, const Vector3D& posMax);
    BBox(const BBox& box);

    ~BBox();

    BBox& operator=(const BBox& box);

    bool intersect(const Ray& ray, double* tMin, double* tMax) const;

    // Enlarge box to contain vertex or box
    void merge(const Vector3D& v);
    void merge(const BBox& box);
    void merge(const Triangle& t);

    // Check if vertex is inside or not
    bool inside(const Vector3D& v) const;

    inline Vector3D posMin() const { return _posMin; }
    inline Vector3D posMax() const { return _posMax; }
};


#endif  // _SPICA_BBOX_H_
