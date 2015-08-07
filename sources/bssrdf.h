#ifndef _BSSRDF_H_
#define _BSSRDF_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef BSSRDF_EXPORT
        #define BSSRDF_DLL __declspec(dllexport)
    #else
        #define BSSRDF_DLL __declspec(dllimport)
    #endif
#else
    #define BSSRDF_DLL
#endif

#include <vector>

#include "vector3d.h"

class BSSRDF;

// ------------------------------------------------------------
// Interface class for BSSRDF
// ------------------------------------------------------------

class BSSRDF_DLL BSSRDFBase {
protected:
    double _eta;

protected:
    explicit BSSRDFBase(double eta = 1.3) : _eta(eta) {}
    explicit BSSRDFBase(const BSSRDFBase& base) : _eta(base._eta) {}
    BSSRDFBase& operator=(const BSSRDFBase& base) { _eta = base._eta; return *this; }

public:
    virtual ~BSSRDFBase() {}
    virtual double Ft(const Vector3D& nornal, const Vector3D& in) const;
    virtual double Fdr() const;
    virtual Vector3D operator()(const double d2) const = 0;
    virtual BSSRDFBase* clone() const = 0;
};


// ------------------------------------------------------------
// BSSRDF with dipole approximation
// ------------------------------------------------------------

class BSSRDF_DLL DipoleBSSRDF : public BSSRDFBase {
private:
    double _A;
    Vector3D _sigmap_t;
    Vector3D _sigma_tr;
    Vector3D _alphap;
    Vector3D _zpos;
    Vector3D _zneg;

private:
    DipoleBSSRDF();
    DipoleBSSRDF(const Vector3D& sigma_a, const Vector3D& sigmap_s, double eta = 1.3);
    DipoleBSSRDF(const DipoleBSSRDF& bssrdf);
    DipoleBSSRDF& operator=(const DipoleBSSRDF& bssrdf);

public:
    static BSSRDF factory(const Vector3D& sigma_a, const Vector3D& sigmap_s, double eta = 1.3);
    Vector3D operator()(const double d2) const override;
    BSSRDFBase* clone() const override;
};

// ------------------------------------------------------------
// BSSRDF with diffuse reflectance function Rd
// ------------------------------------------------------------

class BSSRDF_DLL DiffuseBSSRDF : public BSSRDFBase {
private:
    std::vector<double> _distances;
    std::vector<Vector3D> _colors;

public:
    DiffuseBSSRDF();
    DiffuseBSSRDF(const double eta, const std::vector<double>& distances, const std::vector<Vector3D>& colors);
    DiffuseBSSRDF(const DiffuseBSSRDF& bssrdf);
    DiffuseBSSRDF(DiffuseBSSRDF&& bssrdf);
    DiffuseBSSRDF& operator=(const DiffuseBSSRDF& bssrdf);
    DiffuseBSSRDF& operator=(DiffuseBSSRDF&& bssrdf);

    static BSSRDF factory(const double eta, const std::vector<double>& distances, const std::vector<Vector3D>& colors);
    BSSRDF factory() const;
    Vector3D operator()(const double d2) const override;
    BSSRDFBase* clone() const override;

    int numIntervals() const;
    const std::vector<double>& distances() const;
    const std::vector<Vector3D>& colors() const;

    DiffuseBSSRDF scaled(double sc) const;
    void load(const std::string& filename);
    void save(const std::string& filename) const;
};


// ------------------------------------------------------------
// Abstract BSSRDF class
// ------------------------------------------------------------

class BSSRDF_DLL BSSRDF {
private:
    const BSSRDFBase* _ptr;

public:
    BSSRDF();
    BSSRDF(const BSSRDF& bssrdf);
    BSSRDF(BSSRDF&& bssrdf);
    ~BSSRDF();

    BSSRDF& operator=(const BSSRDF& bssrdf);
    BSSRDF& operator=(BSSRDF&& bssrdf);

    double Ft(const Vector3D& normal, const Vector3D& in) const;
    double Fdr() const;
    Vector3D operator()(const double d2) const;

private:
    explicit BSSRDF(const BSSRDFBase* ptr);
    void nullCheck() const;

// Friend classes
    friend class DipoleBSSRDF;
    friend class DiffuseBSSRDF;
};

#endif  // _BSSRDF_H_
