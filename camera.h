#pragma once

#include <Eigen/Dense>

struct Camera {
    using Transform = Eigen::Transform<float, 3, Eigen::Affine>;
    using Vec3 = Eigen::Vector3f;
    int height, width;
    float focal;
    float center_x, center_y;
    Transform parameter;

    Camera(int W, int H, float f);

    void setPosition(Eigen::Vector3f p);
    void lookThrough(Eigen::Vector3f direction);
    void lookAt(Eigen::Vector3f target);
    Eigen::Vector3f getCameraCenter() const { return parameter.translation(); }
    Eigen::Matrix3f getRotationC2W() const { return parameter.linear(); }
    Eigen::Matrix3f getRotationW2C() const { return parameter.linear().inverse(); }
    Eigen::Vector3f getTranslationW2C() const { return -parameter.linear().inverse() * parameter.translation(); }
    float cx() const { return center_x; }
    float cy() const { return center_y; }
    Eigen::Vector2f getProjection(Vec3 X) const;
    Eigen::Matrix3f rotationMatrix() const;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
