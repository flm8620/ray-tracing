#pragma once

#include <vector>
#include <memory>

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "intersectable.h"
#include "rtree_util.h"
#include "random_texture.h"

struct Material {
    Eigen::Vector3f diffuse_color = Eigen::Vector3f(0.5, 0.5, 0.5);
    std::shared_ptr<RandomTexture> random_diffuse_texture;
    Eigen::Vector3f specular_coeff = Eigen::Vector3f(0.5, 0.5, 0.5);
    float alpha_phong = 2.0;
    bool transparent = false;
    bool mirror = false;
    bool specular = false;
    bool is_fog = false;
    // fog_sigma*ds is the probability of a photon being absorbed by the fog travalling
    // a infinidesimal ds
    float fog_sigma = 0.0;
    Eigen::Vector3f fog_color = Eigen::Vector3f(1.0, 1.0, 1.0);
    float relative_refractive_index = 1.33;
};

struct Sunshine {
    Eigen::Vector3f direction;
    Eigen::Vector3f color;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct PointLight {
    Eigen::Vector3f pos;
    Eigen::Vector3f color;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Lights {
    std::vector<Sunshine> sunshines;
    std::vector<PointLight> pointLights;
    Eigen::Vector3f ambientColor;
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

    Eigen::Vector3f background_color = Eigen::Vector3f(0.f, 0.f, 0.f);

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
    void setAmbientColor(const Eigen::Vector3f &color);
    Eigen::Vector3f getAmbientColor() const;
    Lights getAllLights() const;
    void setBackground(const Eigen::Vector3f &color) { background_color = color; }
    Eigen::Vector3f getBackground() const { return background_color; }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
