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
    , _type(BSDF_TYPE_NONE)
{
}

BSDF::BSDF(const BSDF& brdf)
    : _ptr(NULL)
    , _bssrdf(NULL)
    , _type(BSDF_TYPE_NONE)
{
    this->operator=(brdf);
}

BSDF::BSDF(const BSDFBase* ptr, BsdfType type)
    : _ptr(ptr)
    , _bssrdf(NULL)
    , _type(type)
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
    _type   = bsdf._type;

    if (bsdf._ptr != NULL) {
        _ptr = bsdf._ptr->clone();
    }

    if (bsdf._bssrdf != NULL) {
        _bssrdf = new BSSRDF(*bsdf._bssrdf);
    }

    return *this;
}

const Vector3D& BSDF::reflectance() const {
    return _ptr->reflectance();
}

void BSDF::sample(const Vector3D& in, const Vector3D& normal, const double rand1, const double rand2, Vector3D* out, double* pdf) const {
    _ptr->sample(in, normal, rand1, rand2, out, pdf);
}

BsdfType BSDF::type() const {
    return _type;
}

void BSDF::setBssrdf(const BSSRDF& bssrdf) {
    this->_bssrdf = new BSSRDF(bssrdf);
    this->_type = BSDF_TYPE_BSSRDF;
}
