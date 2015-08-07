#ifndef _PERSPECTIVE_CAMERA_H_
#define _PERSPECTIVE_CAMERA_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef PERSPECTIVE_CAMERA_EXPORT
        #define PERSPECTIVE_CAMERA_DLL __declspec(dllexport)
    #else
        #define PERSPECTIVE_CAMERA_DLL __declspec(dllimport)
    #endif
#else
    #define PERSPECTIVE_CAMERA_DLL
#endif

#include "camera_interface.h"

class PERSPECTIVE_CAMERA_DLL PerspectiveCamera : public ICamera {
private:
    double _fov;
    double _aspect;

    double _halfTangent;

public:
    PerspectiveCamera();
    PerspectiveCamera(const Vector3D& center, const Vector3D& direction, const Vector3D& up, double fov, int imageWidth, int imageHeight, double sensitivity);
    PerspectiveCamera(const PerspectiveCamera& camera);
    ~PerspectiveCamera();

    PerspectiveCamera& operator=(const PerspectiveCamera& camera);

    Ray getRay(double pixelX, double pixelY) const override;
};

typedef PerspectiveCamera Camera;

#endif  // _PERSPECTIVE_CAMERA_H_
