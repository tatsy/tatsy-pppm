#define BSDF_EXPORT
#include "bsdf.h"

#include <cstdlib>

#include "brdf.h"
#include "bssrdf.h"

// --------------------------------------------------
// BSDF
// --------------------------------------------------
BSDF::BSDF()
    : _ptr(NULL)
    , _bssrdf(NULL)
{
}

BSDF::BSDF(const BSDF& brdf)
    : _ptr(NULL)
    , _bssrdf(NULL)
{
    this->operator=(brdf);
}

BSDF::BSDF(const BRDFBase* ptr)
    : _ptr(ptr)
    , _bssrdf(NULL)
{
}

BSDF::~BSDF()
{
    delete _ptr;
    delete _bssrdf;
}

BSDF& BSDF::operator=(const BSDF& bsdf) {
    delete _ptr;
    delete _bssrdf;
    _ptr    = NULL;
    _bssrdf = NULL;

    if (bsdf._ptr != NULL) {
        _ptr = bsdf._ptr->clone();
    }

    if (bsdf._bssrdf != NULL) {
        _bssrdf = new BSSRDF(*bsdf._bssrdf);
    }

    return *this;
}

Vector3D BSDF::reflectance() const {
    return _ptr->reflectance();
}

void BSDF::sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out) const {
    _ptr->sample(in, normal, rand1, rand2, out);
}

BsdfType BSDF::type() const {
    return _ptr->type();
}