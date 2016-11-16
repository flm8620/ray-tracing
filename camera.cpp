#include "camera.h"

Camera::Camera(int W, int H, float f){
    parameter.setIdentity();
    height=H;
    width=W;
    focal=f;
    center_x = width / 2.;
    center_y = height / 2.;
}

void Camera::setHeadUpward(){
    Vec3 ea = parameter.rotation().eulerAngles(2,0,2);
    Eigen::Matrix3f aa;
    const float pi = 3.1415926535897;
    if(ea[1]>0){
        ea[0]+=pi;
        ea[1]=-ea[1];
    }
    aa = Eigen::AngleAxisf(ea[0], Vec3::UnitZ())
            * Eigen::AngleAxisf(ea[1], Vec3::UnitX())
            * Eigen::AngleAxisf(0.0, Vec3::UnitZ());
    parameter = Eigen::Translation3f(parameter.translation()) * aa;
}

void Camera::setPosition(Eigen::Vector3f p){
    parameter.translation() = p;
}

void Camera::lookThrough(Eigen::Vector3f direction){
    Eigen::Matrix3f R = parameter.rotation();
    Vec3 z = R.block(0,2,3,1);
    Eigen::Quaternion<float> q;
    q.setFromTwoVectors(z,direction);
    parameter = Eigen::Translation3f(parameter.translation()) * q;
    setHeadUpward();
}

void Camera::lookAt(Eigen::Vector3f target){
    lookThrough(target - parameter.translation());
}

float Camera::x() const{return parameter.translation()[0];}

float Camera::y() const{return parameter.translation()[1];}

float Camera::z() const{return parameter.translation()[2];}

Eigen::Vector2f Camera::getProjection(Eigen::Vector3f X) const
{
    Eigen::Vector3f EX = parameter.inverse(Eigen::Affine).linear()*X;
    return Eigen::Vector2f(EX[0]/EX[2]*width/focal + center_x, EX[1]/EX[2]*height/focal + center_y);
}

Eigen::Matrix3f Camera::rotationMatrix() const{return parameter.rotation();}
