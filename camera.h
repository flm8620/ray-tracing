#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Dense>

struct Camera
{
    using Transform = Eigen::Transform<float,3,Eigen::Affine>;
    using Vec3 = Eigen::Vector3f;
    int height,width;
    float focal;
    Transform parameter;

    Camera();

    void setHeadUpward(){
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
    void setPosition(Eigen::Vector3f p){
        parameter.translation() = p;
    }
    void lookThrough(Eigen::Vector3f direction){
        Eigen::Matrix3f R = parameter.rotation();
        Vec3 z = R.block(0,2,3,1);
        Eigen::Quaternion<float> q;
        q.setFromTwoVectors(z,direction);
        parameter = Eigen::Translation3f(parameter.translation()) * q;
        setHeadUpward();
    }
    void lookAt(Eigen::Vector3f target){
        lookThrough(target - parameter.translation());
    }
    float x()const{return parameter.translation()[0];}
    float y()const{return parameter.translation()[1];}
    float z()const{return parameter.translation()[2];}
    Eigen::Matrix3f rotationMatrix()const{return parameter.rotation();}

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif // CAMERA_H
