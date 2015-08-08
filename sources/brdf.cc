#define BRDF_EXPORT
#include "brdf.h"

#include <cstdlib>

#include "bsdf.h"
#include "sampler.h"
#include "reflectance.h"

// --------------------------------------------------
// Lambertian BRDF
// --------------------------------------------------

LambertianBRDF::LambertianBRDF(const Vector3D& reflectance)
    : _reflectance(reflectance)
{
}

const Vector3D& LambertianBRDF::reflectance() const {
    return _reflectance;
}

void LambertianBRDF::sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out, double* pdf) const {
    sampler::onHemisphere(normal, out, rand1, rand2);
}

BSDFBase* LambertianBRDF::clone() const {
    return new LambertianBRDF(_reflectance);
}

BSDF LambertianBRDF::factory(const Vector3D& reflectance) {
    return BSDF(new LambertianBRDF(reflectance), BSDF_TYPE_LAMBERTIAN_BRDF);
}

// --------------------------------------------------
// Specular BRDF
// --------------------------------------------------

SpecularBRDF::SpecularBRDF(const Vector3D& reflectance)
    : _reflectance(reflectance)
{
}

const Vector3D& SpecularBRDF::reflectance() const {
    return _reflectance;
}

void SpecularBRDF::sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out, double* pdf) const {
    (*out) = Vector3D::reflect(in, normal);
}

BSDFBase* SpecularBRDF::clone() const {
    return new SpecularBRDF(_reflectance);
}

BSDF SpecularBRDF::factory(const Vector3D& reflectance) {
    return BSDF(new SpecularBRDF(reflectance), BSDF_TYPE_SPECULAR_BRDF);
}

// --------------------------------------------------
// Phong BRDF
// --------------------------------------------------

PhongBRDF::PhongBRDF(const Vector3D& reflectance, const double n)
    : _reflectance(reflectance)
    , _coeffN(n)
{
}

const Vector3D& PhongBRDF::reflectance() const {
    return _reflectance;
}

void PhongBRDF::sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out, double* pdf) const {
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

BSDFBase* PhongBRDF::clone() const {
    return new PhongBRDF(_reflectance, _coeffN);
}

BSDF PhongBRDF::factory(const Vector3D& reflectance, const double n) {
    return BSDF(new PhongBRDF(reflectance, n), BSDF_TYPE_PHONG_BRDF);
}

// --------------------------------------------------
// Refraction BSDF
// --------------------------------------------------

RefractionBSDF::RefractionBSDF(const Vector3D& reflectance) 
    : _reflectance(reflectance)
{
}

const Vector3D& RefractionBSDF::reflectance() const {
    return _reflectance;
}

void RefractionBSDF::sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out, double* pdf) const {
    bool into = true;
    Vector3D orientingNormal = normal;
    if (Vector3D::dot(in, normal) >= 0.0) {
        into = false;
        orientingNormal *= -1.0;
    }

    Vector3D reflectDir, transmitDir;
    double fresnelRe, fresnelTr;
    bool totalReflectance = checkTotalReflection(into, in, normal, orientingNormal, &reflectDir, &transmitDir, &fresnelRe, &fresnelTr);

    if (totalReflectance) {
        *out = reflectDir;
        *pdf = 1.0;
    } else {
        const double reflectProbability = 0.25 + 0.5 * REFLECT_PROBABILITY;
        if (rand1 < reflectProbability) {
            *out = reflectDir;
            *pdf = reflectProbability / fresnelRe;
        } else {
            *out = transmitDir;
            *pdf = (1.0 - reflectProbability) / fresnelTr;
        }
    }
}

BSDFBase* RefractionBSDF::clone() const {
    return new RefractionBSDF(_reflectance);
}

BSDF RefractionBSDF::factory(const Vector3D& reflectance) {
    return BSDF(new RefractionBSDF(reflectance), BSDF_TYPE_REFRACTION);
}