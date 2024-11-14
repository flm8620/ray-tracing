#pragma once

#include <vector>
#include <memory>

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "intersectable.h"
#include "rtree_util.h"

struct Material {
    float diffuse_coeff = 0.5;
    float specular_coeff = 0.5;
    float alpha_phong = 2.0;
    bool transparent = false;
    bool mirror = false;
    bool specular = false;
    float relative_refractive_index = 1.33;
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

    MyTree tree;

    Lights lights;

  public:
    Scene();
    const MyTree& getTree() const { return tree; }
    void addMeshFromPlyFile(const char *file, Material &material);
    void addObject(std::shared_ptr<Intersectable> obj, Material &material);
    void addSunshine(Sunshine s);
    std::vector<Sunshine> getSunshines() const { return lights.sunshines; }
    bool ray_intersect_query(Eigen::Vector3f &rayO, Eigen::Vector3f &rayD, IntersectReport &report, Material &material) const;
    void setAmbientIntensity(float I);
    float getAmbientIntensity() const;
    Lights getAllLights() const;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
