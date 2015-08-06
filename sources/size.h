#ifndef _SIZE_H_
#define _SIZE_H_

#include <type_traits>

extern void* enabler;
template <class T, typename std::enable_if<std::is_arithmetic<T>::value >::type *& = enabler>
class Size {
private:
    T _width;
    T _height;

public:
    Size()
        : _width(0)
        , _height(0)
    {
    }

    Size(T width, T height)
        : _width(width)
        , _height(height)
    {
    }

    Size(const Size& size)
        : _width(size._width)
        , _height(size._height)
    {
    }

    Size& operator=(const Size& size) {
        this->_width = size._width;
        this->_height = size._height;
        return *this;
    }

    inline T width() const { return _width; }
    inline T height() const { return _height; }
};

#endif  // _SIZE_H_