#ifndef GEOMETRY_UTIL_H
#define GEOMETRY_UTIL_H

#include <Eigen/Dense>

//Moller–Trumbore ray-triangle intersection algorithm
template<typename T>
bool ray_triangle_intersect(Eigen::Matrix<T,3,1> v0,
                            Eigen::Matrix<T,3,1> v1,
                            Eigen::Matrix<T,3,1> v2,
                            Eigen::Matrix<T,3,1> rayO,
                            Eigen::Matrix<T,3,1> rayD,
                            T* distance_return,
                            Eigen::Matrix<T,3,1>* intersect_point){
    using Vec = Eigen::Matrix<T,3,1>;
    const T eps = 1e-9;
    Vec e1 = v1 - v0;
    Vec e2 = v2 - v0;

    Vec P = rayD.cross(e2);
    T det = e1.dot(P);
    if(-eps < det && det < eps) return false;

    T inv_det = 1.0/det;
    Vec TT = rayO - v0;
    T u = TT.dot(P) * inv_det;
    if(u < 0.0 || u > 1.0) return false;

    Vec Q = TT.cross(e1);
    T v = rayD.dot(Q) * inv_det;
    if(v < 0.0 || u + v > 1.0) return false;

    T t = e2.dot(Q) * inv_det;
    if(t > eps){
        *distance_return = t;
        *intersect_point = u * v1 + v * v2 + (1-u-v)*v0;
        return true;
    }
    return false;
}


#endif // GEOMETRY_UTIL_H
