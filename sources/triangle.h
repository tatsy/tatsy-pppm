#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "geometry_interface.h"

class Triangle : public IGeometry {
private:
    Vector3D _p0;
    Vector3D _p1;
    Vector3D _p2;

public:
    Triangle();
    Triangle(const Vector3D& p0, const Vector3D& p1, const Vector3D& p2);
    Triangle(const Triangle& tri);
    ~Triangle();

    Triangle& operator=(const Triangle& tri);

    Vector3D gravity() const;
    Vector3D p(int id) const;

    Vector3D normal() const;

    // Compute ray-triangle intersection with Tomas Moller's algorithm
    bool intersect(const Ray& ray, Hitpoint* hitpoint) const override;

    double area() const override;

    IGeometry* clone() const override;

    inline Vector3D p0() const { return _p0; }
    inline Vector3D p1() const { return _p1; }
    inline Vector3D p2() const { return _p2; }
};


#endif  // _TRIANGLE_H_
