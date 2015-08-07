#ifndef _GEOMETRY_INTERFACE_H_
#define _GEOMETRY_INTERFACE_H_

#include <vector>

#include "ray.h"

class Triangle;

// --------------------------------------------------
// Interface class for geometries
// --------------------------------------------------
class IGeometry {    
public:
    IGeometry() {}
    virtual ~IGeometry() {}
    virtual bool intersect(const Ray& ray, Hitpoint* hitpoint) const = 0;
    virtual double area() const = 0;
    virtual IGeometry* clone() const = 0;
    virtual std::vector<Triangle> triangulate() const = 0;
};

#endif  // SPICA_PRIMITIVE_H_
