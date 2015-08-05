#ifndef _GEOMETRY_INTERFACE_H_
#define _GEOMETRY_INTERFACE_H_

#include "ray.h"

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
};

#endif  // SPICA_PRIMITIVE_H_
