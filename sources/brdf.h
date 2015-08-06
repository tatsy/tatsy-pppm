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


#include "common.h"
#include "vector3d.h"

class BRDF;

enum BrdfType {
    BRDF_LAMBERTIAN = 0x01,
    BRDF_SPECULAR,
    BRDF_PHONG
};

// Interface class for BRDF object
class BRDFBase {
protected:

protected:
    BRDFBase() {}
    explicit BRDFBase(const BRDFBase&) {}

public:
    virtual ~BRDFBase() {}
    virtual Vector3D reflectance() const = 0;
    virtual void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const = 0;
    virtual BrdfType type() const = 0;
    virtual BRDFBase* clone() const = 0;
};

class BRDF_DLL LambertianBRDF : public BRDFBase {
private:
    static BrdfType _type;
    Vector3D _reflectance;

public:
    static BRDF factory(const Vector3D& reflectance);
    Vector3D reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const override;
    BrdfType type() const override;
    BRDFBase* clone() const override;

private:
    explicit LambertianBRDF(const Vector3D& reflectance);
};

class BRDF_DLL SpecularBRDF : public BRDFBase {
private:
    static BrdfType _type;
    Vector3D _reflectance;

public:
    static BRDF factory(const Vector3D& reflectance);
    Vector3D reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const override;
    BrdfType type() const override;
    BRDFBase* clone() const override;

private:
    explicit SpecularBRDF(const Vector3D& reflectance);
};

class BRDF_DLL PhongBRDF : public BRDFBase {
private:
    static BrdfType _type;
    Vector3D _reflectance;
    double _coeffN;

public:
    static BRDF factory(const Vector3D& reflectance, const double n);
    Vector3D reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const override;
    BrdfType type() const override;
    BRDFBase* clone() const override;

private:
    PhongBRDF(const Vector3D& reflectance, const double n);
};

class BRDF_DLL BRDF {
private:
    const BRDFBase* _ptr;

public:
    BRDF();
    BRDF(const BRDF& brdf);
    ~BRDF();

    BRDF& operator=(const BRDF& brdf);
    Vector3D reflectance() const;
    void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const;
    BrdfType type() const;

private:
    explicit BRDF(const BRDFBase* ptr);

// Friend classes
    friend class LambertianBRDF;
    friend class SpecularBRDF;
    friend class PhongBRDF;
};

#endif  // _SPICA_BRDF_H_
