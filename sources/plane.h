#ifndef _PLANE_H_
#define _PLANE_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef PLANE_EXPORT
        #define PLANE_DLL __declspec(dllexport)
    #else
        #define PLANE_DLL __declspec(dllimport)
    #endif
#else
    #define PLANE_DLL
#endif

#include "geometry_interface.h"

class PLANE_DLL Plane : public IGeometry {
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
    std::vector<Triangle> triangulate() const override;

    inline double distance() const { return _distance; }
    inline Vector3D normal() const { return _normal; }
};

#endif  // _PLANE_H_
