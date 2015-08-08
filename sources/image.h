#ifndef SPICA_IMAGE_H_
#define SPICA_IMAGE_H_

#include <string>

#include "vector3d.h"

class Image {
private:
    unsigned int _width;
    unsigned int _height;
    Vector3D* _pixels;

public:
    Image();
    Image(int width, int height);
    Image(const Image& image);
    Image(Image&& image);

    ~Image();

    Image& operator=(const Image& image);
    Image& operator=(Image&& image);

    const Vector3D& operator()(int x, int y) const;
    Vector3D& pixel(int x, int y);

    void resize(const int width, const int height);
    void fill(const Vector3D& color);

    // Gamma correction
    // @param[in] gam: gamma value
    // @param[in] inv: if true inverse gamma correction is performed
    void gamma(const double gam, bool inv = false);

    // Tone mapping by [Reinhard et al. 2001]
    void tonemap();

    inline unsigned int width() const { return _width; }
    inline unsigned int height() const { return _height; }

    void load(const std::string& filename);
    void save(const std::string& filename);

private:
    void loadBmp(const std::string& filename);
    void saveBmp(const std::string& filename) const;

    void loadHdr(const std::string& filename);
    void saveHdr(const std::string& filename) const;

    void savePng(const std::string& filename) const;

private:
    void release();
    static double toReal(unsigned char b);
    static unsigned char toByte(double d);
};

#endif  // _IMAGE_H_
