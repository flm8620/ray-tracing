#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <array>
#include <Eigen/Dense>
#include<Eigen/StdVector>
#include "ply_read.h"
#include "RTree.h"

typedef unsigned int faceID;

struct Ray{
    Eigen::Vector3f O,D;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Face{
    Eigen::Vector3f v0,v1,v2;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct IntersectReport{
    Face face;
    faceID faceid;
    Eigen::Vector3f normal;
    Eigen::Vector3f intersect_point;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Sunshine{
    Eigen::Vector3f direction;
    float intensity;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class Scene
{
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertices;
    std::vector<Sunshine> sunshines;
    std::vector<std::array<unsigned int,3>> face_index;

    typedef RTree<faceID, float, 3, float> MyTree;
    MyTree tree;
public:

    Scene();
    void readPlyFile(const char* file);
    bool ray_intersect_query(Ray ray, IntersectReport *report)const;
    void addSunshine(Sunshine s);
    std::vector<Sunshine> getSunshines()const{return sunshines;}
    Face getFace(int id){
        Face f;
        f.v0 = vertices[face_index[id][0]];
        f.v1 = vertices[face_index[id][1]];
        f.v2 = vertices[face_index[id][2]];
        return f;
    }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif // SCENE_H
