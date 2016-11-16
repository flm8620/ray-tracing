#ifndef GEOMETRY_UTIL_H
#define GEOMETRY_UTIL_H

#include <Eigen/Dense>
#include <map>
#include <vector>
#include "intervals.h"
#include <memory>
namespace geo{
typedef std::map<float, Eigen::Vector3f, std::less<float>,
Eigen::aligned_allocator<std::pair<const float, Eigen::Vector3f> > > NormalsMap;

class CSG{

public:
    virtual ~CSG(){}
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                         Eigen::Vector3f& rayD,
                                         DisjointIntervals& interior,
                                         NormalsMap& normalsMap) = 0;
    virtual bool rayIntersect(Eigen::Vector3f& rayO,
                               Eigen::Vector3f& rayD,
                               float& t,
                               Eigen::Vector3f& normal) final;

    virtual void getBoundingBox(float min[3],float max[3])=0;
};

class CSG_Union : CSG{
    std::vector<std::shared_ptr<CSG> > objects;

public:
    CSG_Union(std::vector<std::shared_ptr<CSG> >& objects);
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                         Eigen::Vector3f& rayD,
                                         DisjointIntervals& interior,
                                         NormalsMap& normalsMap);

    virtual void getBoundingBox(float min[3],float max[3]);
};

class CSG_Geometric : CSG{
};

class CSG_Sphere : CSG_Geometric {
    Eigen::Vector3f center;
    float radius;
    float sq_radius;
    CSG_Sphere(Eigen::Vector3f center, float radius);
    virtual void getBoundingBox(float min[3],float max[3]);
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                              Eigen::Vector3f& rayD,
                              DisjointIntervals& interior,
                              NormalsMap& normalsMap);
};
class CSG_Box : CSG_Geometric {
    Eigen::Transform<float,3,Eigen::Affine> origin_transform;
    Eigen::Transform<float,3,Eigen::Affine> origin_transform_inverse;
    Eigen::Vector3f size_min, size_max;
    CSG_Box(Eigen::Vector3f& min,
        Eigen::Vector3f& max,
        Eigen::Transform<float,3,Eigen::Affine> origin_transform = Eigen::Transform<float,3,Eigen::Affine>::Identity());
    virtual void getBoundingBox(float min[3],float max[3]);
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                              Eigen::Vector3f& rayD,
                              DisjointIntervals& interior,
                              NormalsMap& normalsMap);
};
class CSG_Cylinder : CSG_Geometric {
    Eigen::Vector3f center;
    Eigen::Vector3f bottom_center;
    Eigen::Vector3f top_center;
    Eigen::Vector3f direction;
    float radius;
    float height_min, height_max;
    CSG_Cylinder(Eigen::Vector3f center, Eigen::Vector3f direction, float radius, float height_min, float height_max);
    virtual void getBoundingBox(float min[3],float max[3]);
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                              Eigen::Vector3f& rayD,
                              DisjointIntervals& interior,
                              NormalsMap& normalsMap);
};


}

void EigenToArray(Eigen::Vector3f& v, float a[3]);

//Moller–Trumbore ray-triangle intersection algorithm
template<typename T>
bool ray_triangle_intersect(Eigen::Matrix<T,3,1> v0,
                            Eigen::Matrix<T,3,1> v1,
                            Eigen::Matrix<T,3,1> v2,
                            Eigen::Matrix<T,3,1> rayO,
                            Eigen::Matrix<T,3,1> rayD,
                            T* distance_return,
                            Eigen::Matrix<T,3,1>* intersect_point,
                            T* uu, T* vv, T* ww){
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
        *uu=u; *vv=v; *ww=1-u-v;
        return true;
    }
    return false;
}


#endif // GEOMETRY_UTIL_H
