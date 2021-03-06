#ifndef _RENDER_PARAMETERS_H_
#define _RENDER_PARAMETERS_H_

class RenderParameters {
private:
    int _photons;
    int _spp;
    int    _gatherPhotons;
    double _gatherRadius;

public:
    explicit RenderParameters(int photons = 1000000, int spp = 16, int gatherPhotons = 16, double gatherRadius = 16.0)
        : _photons(photons)
        , _spp(spp)
        , _gatherPhotons(gatherPhotons)
        , _gatherRadius(gatherRadius)
    {
    }

    RenderParameters(const RenderParameters& rp)
        : _photons(rp._photons)
        , _spp(rp._spp)
        , _gatherPhotons(rp._gatherPhotons)
        , _gatherRadius(rp._gatherRadius)
    {
    }

    ~RenderParameters() 
    {
    }

    RenderParameters& operator=(const RenderParameters& rp) {
        this->_photons = rp._photons;
        this->_spp = rp._spp;
        this->_gatherPhotons = rp._gatherPhotons;
        this->_gatherRadius = rp._gatherRadius;
        return *this;
    }

    inline int photons() const { return _photons; }
    inline int spp()     const { return _spp; }
    inline int gatherPhotons() const { return _gatherPhotons; }
    inline double gatherRadius() const { return _gatherRadius; }
};

#endif  // _RENDER_PARAMETERS_H_
