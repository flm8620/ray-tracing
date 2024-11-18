#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

struct IntersectReport {
    Eigen::Vector3f normal;
    Eigen::Vector3f intersect_point;
    float t;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class Intersectable {
  public:
    Intersectable() = default;
    virtual ~Intersectable() {}
    virtual Eigen::AlignedBox3f getBoundingBox() const = 0;
    virtual bool rayIntersect(const Eigen::Vector3f &rayO,
                              const Eigen::Vector3f &rayD,
                              IntersectReport &report) const = 0;
};
