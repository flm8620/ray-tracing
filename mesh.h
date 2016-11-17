#ifndef MESH_H
#define MESH_H
#include "intersectable.h"
#include <Eigen/Dense>
#include <vector>
#include "RTree.h"


class Mesh : public Intersectable
{
public:
    struct Face{
        Eigen::Vector3f v0,v1,v2;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    };
private:
    typedef unsigned int faceID;
    typedef unsigned int vertexID;

    struct MeshIntersectReport{
        Face face;
        faceID faceid;
        Eigen::Vector3f normal;
        Eigen::Vector3f intersect_point;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    };
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertices;
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_normals;

    std::vector<std::array<vertexID,3>> face_index;
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> face_normals;
    std::vector<std::vector<faceID>> vertex_to_face;

    typedef RTree<faceID, float, 3, float> MyTree;
    MyTree tree;
public:
    Mesh();
    void readPlyFile(const char* file);
    virtual void getBoundingBox(float min[3],float max[3]) const;
    virtual bool rayIntersect(Eigen::Vector3f& rayO,
                               Eigen::Vector3f& rayD,
                               IntersectReport& report)const override;
    Face getFace(faceID id)const;
    void exportRTreeToPly(const char * file)const;
};

#endif // MESH_H
