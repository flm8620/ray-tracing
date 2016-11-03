#ifndef CAMERA_H
#define CAMERA_H
#include "Eigen/Dense"
struct Camera{
    int height,width;
    float focus;
    Eigen::Transform<float,3,Eigen::Affine> parameter;


};

#endif // CAMERA_H
