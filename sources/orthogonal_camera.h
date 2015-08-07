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

#include "camera_interface.h"

class ORTHOGONAL_CAMERA_DLL OrthogonalCamera : public ICamera {
private:
    Size<int> _viewsize;

public:
    OrthogonalCamera();
    OrthogonalCamera(const Vector3D& center, const Vector3D& direction, const Vector3D& up, double width, double height, int imageWidth, int imageHeight, double sensitivity);
    OrthogonalCamera(const OrthogonalCamera& camera);
    ~OrthogonalCamera();

    OrthogonalCamera& operator=(const OrthogonalCamera& camera);

    Ray getRay(double pixelX, double pixelY) const override;
};

// typedef OrthogonalCamera Camera;

#endif  // _ORTHOGONAL_CAMERA_H_
