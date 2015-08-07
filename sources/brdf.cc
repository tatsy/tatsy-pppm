#define BRDF_EXPORT
#include "brdf.h"

#include <cstdlib>

#include "bsdf.h"
#include "sampler.h"

// --------------------------------------------------
// Lambertian BRDF
// --------------------------------------------------

BsdfType LambertianBRDF::_type = BSDF_TYPE_LAMBERTIAN_BRDF;

LambertianBRDF::LambertianBRDF(const Vector3D& reflectance)
    : _reflectance(reflectance)
{
}

Vector3D LambertianBRDF::reflectance() const {
    return _reflectance;
}

void LambertianBRDF::sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const {
    sampler::onHemisphere(normal, out, rand1, rand2);
}

BsdfType LambertianBRDF::type() const {
    return _type;
}

BRDFBase* LambertianBRDF::clone() const {
    return new LambertianBRDF(_reflectance);
}

BSDF LambertianBRDF::factory(const Vector3D& reflectance) {
    return BSDF(new LambertianBRDF(reflectance));
}

// --------------------------------------------------
// Specular BRDF
// --------------------------------------------------

BsdfType SpecularBRDF::_type = BSDF_TYPE_SPECULAR_BRDF;

SpecularBRDF::SpecularBRDF(const Vector3D& reflectance)
    : _reflectance(reflectance)
{
}

Vector3D SpecularBRDF::reflectance() const {
    return _reflectance;
}

void SpecularBRDF::sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const {
    (*out) = Vector3D::reflect(in, normal);
}

BsdfType SpecularBRDF::type() const {
    return _type;
}

BRDFBase* SpecularBRDF::clone() const {
    return new SpecularBRDF(_reflectance);
}

BSDF SpecularBRDF::factory(const Vector3D& reflectance) {
    return BSDF(new SpecularBRDF(reflectance));
}

// --------------------------------------------------
// Phong BRDF
// --------------------------------------------------

BsdfType PhongBRDF::_type = BSDF_TYPE_PHONG_BRDF;

PhongBRDF::PhongBRDF(const Vector3D& reflectance, const double n)
    : _reflectance(reflectance)
    , _coeffN(n)
{
}

Vector3D PhongBRDF::reflectance() const {
    return _reflectance;
}

void PhongBRDF::sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const {
    Vector3D refDir = Vector3D::reflect(in, normal);

    Vector3D u, v, w;
    w = refDir;
    if (std::abs(w.x()) > EPS) {
        u = Vector3D::cross(Vector3D(0.0, 1.0, 0.0), w).normalized();
    }
    else {
        u = Vector3D::cross(Vector3D(1.0, 0.0, 0.0), w).normalized();
    }
    v = Vector3D::cross(w, u);

    double theta = acos(pow(rand1, 1.0 / (_coeffN + 1.0)));
    double phi = 2.0 * PI * rand2;

    (*out) = u * sin(theta) * cos(phi) + w * cos(theta) + v * sin(theta) * sin(phi);
}

BsdfType PhongBRDF::type() const {
    return _type;
}

BRDFBase* PhongBRDF::clone() const {
    return new PhongBRDF(_reflectance, _coeffN);
}

BSDF PhongBRDF::factory(const Vector3D& reflectance, const double n) {
    return BSDF(new PhongBRDF(reflectance, n));
}
