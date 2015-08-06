#ifndef _ORTHOGONAL_CAMERA_H_
#define _ORTHOGONAL_CAMERA_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef ORTHOGONAL_CAMERA_EXPORT
        #define ORTHOGONAL_CAMERA_DLL __declspec(dllexport)
    #else
        #define ORTHOGONAL_CAMERA_DLL __declspec(dllimport)
    #endif
#else
    #define ORTHOGONAL_CAMERA_DLL
#endif

#include "vector3d.h"
#include "size.h"
#include "ray.h"

class ORTHOGONAL_CAMERA_DLL OrthogonalCamera {
private:
    Vector3D _center;
    Vector3D _direction;
    Size<double> _viewsize;
    Size<int> _imagesize;

    Vector3D _unitU;
    Vector3D _unitV;

public:
    OrthogonalCamera();
    OrthogonalCamera(const Vector3D& center, const Vector3D& direction, double width, double height, int imageWidth, int imageHeight);
    OrthogonalCamera(const OrthogonalCamera& camera);
    ~OrthogonalCamera();

    OrthogonalCamera& operator=(const OrthogonalCamera& camera);

    Ray getRay(double pixelX, double pixelY) const;

    inline const Vector3D& center() const { return _center; }
    inline const Vector3D& direction() const { return _direction; }
    inline const Size<double>& viewsize() const { return _viewsize; }
    inline const Size<int>& imagesize() const { return _imagesize; }
};

typedef OrthogonalCamera Camera;

#endif  // _ORTHOGONAL_CAMERA_H_
