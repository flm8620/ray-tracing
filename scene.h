#pragma once

#include <vector>
#include <memory>

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "intersectable.h"
#include "RTree.h"

struct Material {
    float diffuse_coeff;
    float specular_coeff;
    float alpha_phong;
    bool transparent;
    bool mirror;
    bool specular;
    float relative_refractive_index;
    Material() {
        diffuse_coeff = 0.5;
        specular_coeff = 0.5;
        alpha_phong = 2.0;
        transparent = false;
        mirror = false;
        specular = false;
        relative_refractive_index = 1.33;
    }
};

struct Sunshine {
    Eigen::Vector3f direction;
    float intensity;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct PointLight {
    Eigen::Vector3f pos;
    float intensity;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Lights {
    std::vector<Sunshine> sunshines;
    std::vector<PointLight> pointLights;
    float ambientIntensity;
};

class Scene {
    typedef unsigned int objectID;
    typedef unsigned int materialID;
    std::vector<std::shared_ptr<Intersectable>> objects;

    std::vector<Material> materials;
    std::vector<materialID> objects_material;

    typedef RTree<objectID, float, 3, float> MyTree;
    MyTree tree;

    Lights lights;

  public:
    Scene();
    void addMeshFromPlyFile(const char *file, Material &material);
    void addObject(std::shared_ptr<Intersectable> obj, Material &material);
    void addSunshine(Sunshine s);
    std::vector<Sunshine> getSunshines() const { return lights.sunshines; }
    bool ray_intersect_query(Eigen::Vector3f &rayO, Eigen::Vector3f &rayD, IntersectReport &report, Material &material) const;
    void setAmbientIntensity(float I);
    float getAmbientIntensity() const;
    Lights getAllLights() const;
    void exportRTreeToPly(const char *file);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
