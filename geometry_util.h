#pragma once

#include <Eigen/Dense>

// Moller–Trumbore ray-triangle intersection algorithm
template <typename T>
bool ray_triangle_intersect(const Eigen::Matrix<T, 3, 1> &v0,
                            const Eigen::Matrix<T, 3, 1> &v1,
                            const Eigen::Matrix<T, 3, 1> &v2,
                            const Eigen::Matrix<T, 3, 1> &rayO,
                            const Eigen::Matrix<T, 3, 1> &rayD,
                            T *distance_return,
                            Eigen::Matrix<T, 3, 1> *intersect_point,
                            T *uu, T *vv, T *ww) {
    using Vec = Eigen::Matrix<T, 3, 1>;
    const T eps = 1e-9;
    Vec e1 = v1 - v0;
    Vec e2 = v2 - v0;

    Vec P = rayD.cross(e2);
    T det = e1.dot(P);
    if (-eps < det && det < eps)
        return false;

    T inv_det = 1.0 / det;
    Vec TT = rayO - v0;
    T u = TT.dot(P) * inv_det;
    if (u < 0.0 || u > 1.0)
        return false;

    Vec Q = TT.cross(e1);
    T v = rayD.dot(Q) * inv_det;
    if (v < 0.0 || u + v > 1.0)
        return false;

    T t = e2.dot(Q) * inv_det;
    if (t > eps) {
        *distance_return = t;
        *intersect_point = u * v1 + v * v2 + (1 - u - v) * v0;
        *uu = u;
        *vv = v;
        *ww = 1 - u - v;
        return true;
    }
    return false;
}
