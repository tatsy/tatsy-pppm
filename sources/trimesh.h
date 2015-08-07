#ifndef _TRIMESH_H_
#define _TRIMESH_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef TRIMESH_EXPORT
        #define TRIMESH_DLL __declspec(dllexport)
    #else
        #define TRIMESH_DLL __declspec(dllimport)
    #endif
#else
    #define TRIMESH_DLL 
#endif

#include <cassert>
#include <string>
#include <memory>
#include <vector>
#include <array>
#include <tuple>

#include "geometry_interface.h"
#include "triangle.h"
#include "bbox.h"
#include "plane.h"

#include "vector3d.h"
#include "qbvh_accel.h"

class Triplet {
private:
    std::array<int,3> _data;

public:
    Triplet()
        : _data()
    {
    }

    Triplet(int i, int j, int k)
        : _data()
    {
        _data[0] = i;
        _data[1] = j;
        _data[2] = k;
    }

    Triplet(const Triplet& triplet)
        : _data()
    {
        this->operator=(triplet);
    }

    ~Triplet()
    {
    }

    Triplet& operator=(const Triplet& triplet) {
        this->_data = triplet._data;
        return *this;
    }

    int operator[](int i) const {
        assert(0 <= i && i <= 2 && "access index out of bounds!");
        return _data[i];
    }    
};
    
class TRIMESH_DLL Trimesh : public IGeometry {
private:
    std::vector<Vector3D> _vertices;
    std::vector<Vector3D> _normals;
    std::vector<Triplet> _faces;
    std::shared_ptr<QBVHAccel> _accel;

public:
    // Contructor
    Trimesh();

    // Constructor
    // @param[in] filename: .ply or .obj file
    explicit Trimesh(const std::string& filename);

    // Constructor
    // @param[in] vertices: vertices to form trimesh
    // @param[in] faceIDs: face IDs (stating from 0)
    Trimesh(const std::vector<Vector3D>& vertices, const std::vector<Triplet>& faceIDs);

    // Copy constructor
    Trimesh(const Trimesh& trimesh);

    // Move constructor
    Trimesh(Trimesh&& trimesh);

    virtual ~Trimesh();

    Trimesh& operator=(const Trimesh& trimesh);
    Trimesh& operator=(Trimesh&& trimesh);

    // Check intersection
    // @param[in] ray: input to check intersect with
    // @param[out] hitpoint: hit point if intersected
    bool intersect(const Ray& ray, Hitpoint* hitpoint) const override;

    // Area of trimesh
    double area() const override;

    // Cloning trimesh
    IGeometry* clone() const override;

    // Triangulate mesh
    std::vector<Triangle> triangulate() const override;

    // Build accelerator structure
    void buildAccel();

    // Load mesh file
    // @param[in] filename: .obj or .ply file
    void load(const std::string& filename);

    // Translate the mesh
    // @param[in] transtion delta
    void translate(const Vector3D& move);

    // Scale the mesh
    // @param[in] scaleX: scaling along X-axis
    // @param[in] scaleY: scaling along Y-axis
    // @param[in] scaleZ: scaling along Z-axis
    void scale(const double scaleX, const double scaleY, const double scaleZ);

    // Scale the mesh
    // @param[in] scaleAll: scaling all the axes
    void scale(const double scaleAll);

    // Put the mesh on the specified plane
    // @param[in] plane: a plane on which the mesh is put
    void putOnPlane(const Plane& plane);

    // Scale and translate the mesh to fit the bounding box
    // @param[in] bbox: target bounding box
    void fitToBBox(const BBox& bbox);

    // Get a triangle with specified ID
    // @param[in] faceID: ID of the triangle to get
    Triangle getTriangle(int faceID) const;

    // Get vertex indices
    std::vector<Triplet> getIndices() const;

    // Get the vertex with specified ID
    // @param[in] vertexID: ID of the vertex
    Vector3D getVertex(int vertexID) const;

    // Get the normal of a face with specified ID
    // @param[in] faceID: ID of the triangle to get a normal
    Vector3D getNormal(int faceID) const;

    // Get the number of vertices
    inline size_t numVerts() const { return _vertices.size(); }

    // Get the number of faces (triangles)
    inline size_t numFaces() const { return _faces.size(); }

private:
    void loadPly(const std::string& filename);
    void loadObj(const std::string& filename);
};

#endif  // _SPICA_TRIMESH_H_
