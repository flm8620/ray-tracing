#include "camera.h"

Camera::Camera(int W, int H, float f) {
    parameter.setIdentity();
    height = H;
    width = W;
    focal = f;
    center_x = width / 2.;
    center_y = height / 2.;
}

void Camera::setPosition(Eigen::Vector3f p) {
    parameter.translation() = p;
}

void Camera::lookThrough(Eigen::Vector3f direction) {
    Eigen::Matrix3f R;
    Vec3 z = direction.normalized();
    Vec3 x = Vec3(0, 0, -1).cross(z).normalized();
    Vec3 y = z.cross(x);
    R << x, y, z;
    parameter.linear() = R;
}

void Camera::lookAt(Eigen::Vector3f target) {
    lookThrough(target - parameter.translation());
}

Eigen::Matrix3f Camera::rotationMatrix() const { return parameter.rotation(); }
