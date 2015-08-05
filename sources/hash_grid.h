#ifndef _HASH_GRID_H_
#define _HASH_GRID_H_

#include <vector>

#include "bbox.h"

template <class Ty>
class HashGrid {
private:
    int _hashSize;
    BBox _bbox;
    double _hashScale;
    std::vector<std::vector<Ty> > _data;

public:
    HashGrid();
    ~HashGrid();

    // Construct hash grid
    // @param[in] points: points stored to the grid
    // @param[in] imageW: image width used to determine the initial grid size (option)
    // @param[in] imageH: image height used to determine the initial grid size (option)
    void construct(std::vector<Ty>& points, const int imageW = -1, const int imageH = -1);

    // Initialize grid
    void init(const int hashSize, const double hashScale, const BBox& bbox);

    // Set point data for the cells inside the specifed bounding box
    void add(const Ty& p, const Vector3D& boxMin, const Vector3D& boxMax);

    // Clear grid data
    void clear();

    std::vector<Ty>& operator[](const Vector3D& v);

private:
    unsigned int hash(const int ix, const int iy, const int iz) const;
};

#include "hash_grid_detail.h"

#endif  // _HASH_GRID_H_

