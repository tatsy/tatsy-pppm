#ifndef _BSDF_H_
#define _BSDF_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef BSDF_EXPORT
        #define BSDF_DLL __declspec(dllexport)
    #else
        #define BSDF_DLL __declspec(dllimport)
    #endif
#else
    #define BSDF_DLL
#endif

#include "vector3d.h"

class BRDFBase;
class BSSRDF;

enum BsdfType {
    BSDF_TYPE_LAMBERTIAN_BRDF,
    BSDF_TYPE_SPECULAR_BRDF,
    BSDF_TYPE_PHONG_BRDF,
    BSDF_TYPE_BSSRDF
};

class BSDF_DLL BSDF {
private:
    const BRDFBase* _ptr;
    const BSSRDF* _bssrdf;

public:
    BSDF();
    BSDF(const BSDF& brdf);
    ~BSDF();

    BSDF& operator=(const BSDF& brdf);
    Vector3D reflectance() const;
    void sample(const Vector3D& in, const Vector3D& orientNormal, const double rand1, const double rand2, Vector3D* out) const;
    BsdfType type() const;

private:
    explicit BSDF(const BRDFBase* ptr);

    // Friend classes
    friend class LambertianBRDF;
    friend class SpecularBRDF;
    friend class PhongBRDF;
    friend class SubsurfaceIntegrator;
};

#endif  // _BSDF_H_
