#ifndef _BRDF_H_
#define _BRDF_H_

#include "common.h"
#include "vector3d.h"

class BRDF;

// Interface class for BRDF object
class BRDFBase {
protected:
    BRDFBase() {}
    explicit BRDFBase(const BRDFBase&) {}

public:
    virtual ~BRDFBase() {}
    virtual Vector3D reflectance() const = 0;
    virtual void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const = 0;
    virtual BRDFBase* clone() const = 0;
};

class LambertianBRDF : public BRDFBase {
private:
    Vector3D _reflectance;

public:
    static BRDF factory(const Vector3D& reflectance);
    Vector3D reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const override;
    BRDFBase* clone() const override;

private:
    explicit LambertianBRDF(const Vector3D& reflectance);
};

class SpecularBRDF : public BRDFBase {
private:
    Vector3D _reflectance;

public:
    static BRDF factory(const Vector3D& reflectance);
    Vector3D reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const override;
    BRDFBase* clone() const override;

private:
    explicit SpecularBRDF(const Vector3D& reflectance);
};

class PhongBRDF : public BRDFBase {
private:
    Vector3D _reflectance;
    double _coeffN;

public:
    static BRDF factory(const Vector3D& reflectance, const double n);
    Vector3D reflectance() const override;
    void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const override;
    BRDFBase* clone() const override;

private:
    PhongBRDF(const Vector3D& reflectance, const double n);
};

class BRDF {
private:
    const BRDFBase* _ptr;

public:
    BRDF();
    BRDF(const BRDF& brdf);
    ~BRDF();

    BRDF& operator=(const BRDF& brdf);
    Vector3D reflectance() const;
    void sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const;

private:
    explicit BRDF(const BRDFBase* ptr);

// Friend classes
    friend class LambertianBRDF;
    friend class SpecularBRDF;
    friend class PhongBRDF;
};

#endif  // _SPICA_BRDF_H_
