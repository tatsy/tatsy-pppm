#ifndef _RENDER_PARAMETERS_H_
#define _RENDER_PARAMETERS_H_

class RenderParameters {
private:
    int _photons;
    int _spp;
    double _gatherRadius;

public:
    explicit RenderParameters(int photons = 1000000, int spp = 16, double gatherRadius = 5.0)
        : _photons(photons)
        , _spp(spp)
        , _gatherRadius(gatherRadius)
    {
    }

    RenderParameters(const RenderParameters& rp)
        : _photons(rp._photons)
        , _spp(rp._spp)
        , _gatherRadius(rp._gatherRadius)
    {
    }

    ~RenderParameters() 
    {
    }

    RenderParameters& operator=(const RenderParameters& rp) {
        this->_photons = rp._photons;
        this->_spp = rp._spp;
        this->_gatherRadius = rp._gatherRadius;
        return *this;
    }

    inline int photons() const { return _photons; }
    inline int spp()     const { return _spp; }
    inline double gatherRadius() const { return _gatherRadius; }
};

#endif  // _RENDER_PARAMETERS_H_
