#ifndef _RENDER_PARAMETERS_H_
#define _RENDER_PARAMETERS_H_

class RenderParameters {
private:
    int _width;
    int _height;
    int _photons;
    int _spp;

public:
    explicit RenderParameters(int width = 800, int height = 600, int photons = 1000000, int spp = 16)
        : _width(width) 
        , _height(height)
        , _photons(photons)
        , _spp(spp)
    {
    }

    RenderParameters(const RenderParameters& rp)
        : _width(rp._width)
        , _height(rp._height)
        , _photons(rp._photons)
        , _spp(rp._spp)
    {
    }

    ~RenderParameters() 
    {
    }

    RenderParameters& operator=(const RenderParameters& rp) {
        this->_width = rp._width;
        this->_height = rp._height;
        this->_photons = rp._photons;
        this->_spp = rp._spp;
        return *this;
    }

    inline int width()   const { return _width; }
    inline int height()  const { return _height; }
    inline int photons() const { return _photons; }
    inline int spp()     const { return _spp; }
};

#endif  // _RENDER_PARAMETERS_H_
