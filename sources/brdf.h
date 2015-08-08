#ifndef _BRDF_H_
#define _BRDF_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef BRDF_EXPORT
        #define BRDF_DLL __declspec(dllexport)
    #else
        #define BRDF_DLL __declspec(dllimport)
    #endif
#else
    #define BRDF_DLL
#endif

#include "readonly_interface.h"
#include "bsdf.h"
#include "common.h"
#include "vector3d.h"

// --------------------------------------------------
// Interface class for BSDFs
// --------------------------------------------------
class BSDFBase : private IReadOnly {
protected:
    BSDFBase() {}
    explicit BSDFBase(const BSDFBase&) {}

public:
    virtual ~BSDFBase() {}
    virtual const Vector3D& reflectance() const = 0;
    virtual void sample(const Vector3D& in, const Vector3D& orientNormal, const double rand1, const double rand2, Vector3D* out, double* pdf) const = 0;
    virtual BSDFBase* clone() const = 0;
};

// --------------------------------------------------
// Lambertian BRDF
// --------------------------------------------------
class BRDF_DLL LambertianBRDF : public BSDFBase {
private:
    Vector3D _reflectance;

public:
    static BSDF factory(const Vector3D& reflectance);
    const Vector3D& reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& orinentNormal, const double rand1, const double rand2, Vector3D* out, double* pdf) const override;
    BSDFBase* clone() const override;

private:
    explicit LambertianBRDF(const Vector3D& reflectance);
};

// --------------------------------------------------
// Specular BRDF
// --------------------------------------------------
class BRDF_DLL SpecularBRDF : public BSDFBase {
private:
    Vector3D _reflectance;

public:
    static BSDF factory(const Vector3D& reflectance);
    const Vector3D& reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& orientNormal, const double rand1, const double rand2, Vector3D* out, double* pdf) const override;
    BSDFBase* clone() const override;

private:
    explicit SpecularBRDF(const Vector3D& reflectance);
};

// --------------------------------------------------
// Phong BRDF
// --------------------------------------------------
class BRDF_DLL PhongBRDF : public BSDFBase {
private:
    Vector3D _reflectance;
    double _coeffN;

public:
    static BSDF factory(const Vector3D& reflectance, const double n);
    const Vector3D& reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& orientNormal, const double rand1, const double rand2, Vector3D* out, double* pdf) const override;
    BSDFBase* clone() const override;

private:
    PhongBRDF(const Vector3D& reflectance, const double n);
};

// --------------------------------------------------
// Refraction BSDF
// --------------------------------------------------
class BRDF_DLL RefractionBSDF : public BSDFBase {
private:
    Vector3D _reflectance;

public:
    static BSDF factory(const Vector3D& reflectance);
    const Vector3D& reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& orientNormal, const double rand1, const double rand2, Vector3D* out, double* pdf) const override;
    BSDFBase* clone() const override;

private:
    explicit RefractionBSDF(const Vector3D& reflectance);
};

#endif  // _SPICA_BRDF_H_
