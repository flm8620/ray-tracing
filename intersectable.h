#ifndef INTERSECTABLE_H
#define INTERSECTABLE_H
#include <Eigen/Dense>

struct IntersectReport{
    Eigen::Vector3f normal;
    Eigen::Vector3f intersect_point;
    float t;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class Intersectable
{
public:
    Intersectable();
    virtual ~Intersectable(){}
    virtual void getBoundingBox(float min[3],float max[3]) const=0;
    virtual bool rayIntersect(Eigen::Vector3f& rayO,
                               Eigen::Vector3f& rayD,
                               IntersectReport& report) const= 0;
};

#endif // INTERSECTABLE_H
