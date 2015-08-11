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

class BSDFBase;
class BSSRDF;

enum BsdfType {
    BSDF_TYPE_NONE = 0x00,
    BSDF_TYPE_LAMBERTIAN_BRDF = 0x01,
    BSDF_TYPE_SPECULAR_BRDF = 0x02,
    BSDF_TYPE_PHONG_BRDF = 0x04,
    BSDF_TYPE_REFRACTION = 0x08,
    BSDF_TYPE_BSSRDF = 0x10
};

inline BsdfType operator|(BsdfType a, BsdfType b) {
    return static_cast<BsdfType>((int)a | (int)b);
}

class BSDF_DLL BSDF {
private:
    const BSDFBase* _ptr;
    const BSSRDF*   _bssrdf;
    BsdfType        _type;

public:
    BSDF();
    BSDF(const BSDF& brdf);
    ~BSDF();

    BSDF& operator=(const BSDF& brdf);
    const Vector3D& reflectance() const;
    void sample(const Vector3D& in, const Vector3D& orientNormal, const double rand1, const double rand2, Vector3D* out, double* pdf) const;
    BsdfType type() const;

    void setBssrdf(const BSSRDF& bssrdf);

private:
    BSDF(const BSDFBase* ptr, BsdfType type);

    // Friend classes
    friend class LambertianBRDF;
    friend class SpecularBRDF;
    friend class PhongBRDF;
    friend class RefractionBSDF;
    friend class SubsurfaceIntegrator;
};

#endif  // _BSDF_H_
