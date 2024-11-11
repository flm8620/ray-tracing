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

    void setHeadUpward();
    void setPosition(Eigen::Vector3f p);
    void lookThrough(Eigen::Vector3f direction);
    void lookAt(Eigen::Vector3f target);
    float x() const;
    float y() const;
    float z() const;
    Eigen::Vector2f getProjection(Vec3 X) const;
    Eigen::Matrix3f rotationMatrix() const;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
