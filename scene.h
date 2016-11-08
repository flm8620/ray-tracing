#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <array>
#include <Eigen/Dense>
#include<Eigen/StdVector>
#include "ply_read.h"
#include "RTree.h"

typedef unsigned int faceID;
typedef unsigned int vertexID;
struct Ray{
    Eigen::Vector3f O,D;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Material{
    float diffuse_coeff;
    float specular_coeff;
    float alpha_phong;
    bool transparent;
    bool mirror;
    bool specular;
    float relative_refractive_index;
    Material(){
        diffuse_coeff = 0.5;
        specular_coeff = 0.5;
        alpha_phong = 2.0;
        transparent = false;
        mirror = false;
        specular = false;
        relative_refractive_index = 1.33;
    }

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
struct PointLight{
    Eigen::Vector3f pos;
    float intensity;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Lights{
    std::vector<Sunshine> sunshines;
    std::vector<PointLight> pointLights;
    float ambientIntensity;
};

class Scene
{
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertices;
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_normals;

    std::vector<std::array<vertexID,3>> face_index;
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> face_normals;
    std::vector<std::vector<faceID>> vertex_to_face;

    std::vector<Material> materials;
    std::vector<unsigned int> face_material;

    typedef RTree<faceID, float, 3, float> MyTree;
    MyTree tree;

    Lights lights;

public:

    Scene();
    void readPlyFile(const char* file, Material m);
    bool ray_intersect_query(Ray ray, IntersectReport *report)const;
    void addSunshine(Sunshine s);
    std::vector<Sunshine> getSunshines()const{return lights.sunshines;}
    Face getFace(faceID id);
    const Material& getFaceMaterial(faceID id) const;
    void setAmbientIntensity(float I);
    float getAmbientIntensity()const;
    Lights getAllLights()const;
    void exportRTreeToPly(const char * file);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif // SCENE_H
