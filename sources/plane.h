#ifndef _PLANE_H_
#define _PLANE_H_

#include "geometry_interface.h"

class Plane : public IGeometry {
protected:
    double _distance;
    Vector3D _normal;

public:
    Plane();
    Plane(double distance, const Vector3D& normal);
    Plane(const Plane& plane);
    virtual ~Plane();

    Plane& operator=(const Plane& plane);

    bool intersect(const Ray& ray, Hitpoint* hitpoint) const override;

    double area() const override;
    IGeometry* clone() const override;

    inline double distance() const { return _distance; }
    inline Vector3D normal() const { return _normal; }
};

#endif  // _PLANE_H_
