#pragma once

#include <vector>
#include <memory>

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "intersectable.h"
#include "rtree_util.h"
#include "random_texture.h"

struct Material {
    float diffuse_coeff = 0.5;
    std::shared_ptr<RandomNoise> random_diffuse_texture;
    float specular_coeff = 0.5;
    float alpha_phong = 2.0;
    bool transparent = false;
    bool mirror = false;
    bool specular = false;
    bool is_fog = false;
    // fog_sigma*ds is the probability of a photon being absorbed by the fog travalling
    // a infinidesimal ds
    float fog_sigma = 0.0;
    float fog_color = 1.0;
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

struct IntervalIntersectReport {
    std::vector<DisjointIntervals> objs_intervals;
    std::vector<Material *> materials;
};

class Scene {
    typedef unsigned int objectID;
    typedef unsigned int materialID;
    std::vector<std::shared_ptr<Intersectable>> objects;

    std::vector<std::shared_ptr<Material>> materials;
    std::vector<materialID> objects_material;

    MyTree tree;

    Lights lights;

    float background_color = 0.0f;

  public:
    Scene();
    const MyTree &getTree() const { return tree; }
    void addMeshFromPlyFile(const char *file, const std::shared_ptr<Material> &material);
    void addObject(std::shared_ptr<Intersectable> obj, const std::shared_ptr<Material> &material);
    void addSunshine(Sunshine s);
    std::vector<Sunshine> getSunshines() const { return lights.sunshines; }
    bool ray_intersect_query(const Eigen::Vector3f &rayO, const Eigen::Vector3f &rayD,
                             IntersectReport &report, Material **material,
                             IntervalIntersectReport *interval_report = nullptr) const;
    void setAmbientIntensity(float I);
    float getAmbientIntensity() const;
    Lights getAllLights() const;
    void setBackground(float color) { background_color = color; }
    float getBackground() const { return background_color; }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
