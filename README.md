sppm-for-rt3
===

> Stochastic progressive photon mapping renderer for "Ray tracing camp 3!!!"

## Installation

```shell
$ git clone https://github.com/tatsy/photon-diffusion.git
$ cmake .
$ cmake --build .
```

## Overview

**Integrator:** stochastic progressive photon mapping [Hachiska et al. 2008]
**BSSRDF:** dipole approximation model [Jensen et al. 2001]
**Sampler:** Halton sampler

## Acknowledgment

The author sincerely appreciates the following websites for asset data.

**Asian dragon (PLY mesh model)**
* The Stanford 3D Scanning Repository: http://graphics.stanford.edu/data/3Dscanrep/

**Suway station (HDR image for IBL)**
* sIBL Archive: http://www.hdrlabs.com/sibl/archive.html

## License

MIT License 2015 (c) tatsy, Tatsuya Yatagawa
