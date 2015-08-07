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

#include "bsdf.h"
#include "common.h"
#include "vector3d.h"

// Interface class for BRDF object
class BRDFBase {
protected:
    BRDFBase() {}
    explicit BRDFBase(const BRDFBase&) {}

public:
    virtual ~BRDFBase() {}
    virtual Vector3D reflectance() const = 0;
    virtual void sample(const Vector3D& in, const Vector3D& orientNormal, const double rand1, const double rand2, Vector3D* out) const = 0;
    virtual BRDFBase* clone() const = 0;
};

class BRDF_DLL LambertianBRDF : public BRDFBase {
private:
    Vector3D _reflectance;

public:
    static BSDF factory(const Vector3D& reflectance);
    Vector3D reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& orinentNormal, const double rand1, const double rand2, Vector3D* out) const override;
    BRDFBase* clone() const override;

private:
    explicit LambertianBRDF(const Vector3D& reflectance);
};

class BRDF_DLL SpecularBRDF : public BRDFBase {
private:
    Vector3D _reflectance;

public:
    static BSDF factory(const Vector3D& reflectance);
    Vector3D reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& orientNormal, const double rand1, const double rand2, Vector3D* out) const override;
    BRDFBase* clone() const override;

private:
    explicit SpecularBRDF(const Vector3D& reflectance);
};

class BRDF_DLL PhongBRDF : public BRDFBase {
private:
    Vector3D _reflectance;
    double _coeffN;

public:
    static BSDF factory(const Vector3D& reflectance, const double n);
    Vector3D reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& orientNormal, const double rand1, const double rand2, Vector3D* out) const override;
    BRDFBase* clone() const override;

private:
    PhongBRDF(const Vector3D& reflectance, const double n);
};


#endif  // _SPICA_BRDF_H_
