#ifndef _RENDER_PARAMETERS_H_
#define _RENDER_PARAMETERS_H_

class RenderParameters {
private:
    int _photons;
    int _spp;

public:
    explicit RenderParameters(int photons = 1000000, int spp = 16)
        : _photons(photons)
        , _spp(spp)
    {
    }

    RenderParameters(const RenderParameters& rp)
        : _photons(rp._photons)
        , _spp(rp._spp)
    {
    }

    ~RenderParameters() 
    {
    }

    RenderParameters& operator=(const RenderParameters& rp) {
        this->_photons = rp._photons;
        this->_spp = rp._spp;
        return *this;
    }

    inline int photons() const { return _photons; }
    inline int spp()     const { return _spp; }
};

#endif  // _RENDER_PARAMETERS_H_
