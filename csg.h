#ifndef CSG_H
#define CSG_H
#include "intersectable.h"

#include <map>
#include <vector>
#include <memory>
#include "intervals.h"
typedef std::map<float, Eigen::Vector3f, std::less<float>,
Eigen::aligned_allocator<std::pair<const float, Eigen::Vector3f> > > NormalsMap;

void EigenToArray(Eigen::Vector3f const & v, float a[3]);

class CSG : public Intersectable{

public:
    virtual ~CSG(){}
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior,
                                       NormalsMap& normalsMap) const= 0;
    virtual bool rayIntersect(Eigen::Vector3f& rayO,
                              Eigen::Vector3f& rayD,
                              IntersectReport& report) const final;
};

class CSG_Union : public CSG{
    std::vector<std::shared_ptr<CSG> > objects;

public:
    CSG_Union(std::vector<std::shared_ptr<CSG> >& objects);
    virtual void getBoundingBox(float min[3],float max[3])const override;
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior,
                                       NormalsMap& normalsMap) const override;
};

class CSG_Geometric : public CSG{
};

class CSG_Sphere : public CSG_Geometric {
    Eigen::Vector3f center;
    float radius;
    float sq_radius;
public:
    CSG_Sphere(Eigen::Vector3f center, float radius);
    virtual void getBoundingBox(float min[3],float max[3])const override;
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior,
                                       NormalsMap& normalsMap) const override;
};
class CSG_Box : public CSG_Geometric {
    Eigen::Transform<float,3,Eigen::Affine> origin_transform;
    Eigen::Transform<float,3,Eigen::Affine> origin_transform_inverse;
    Eigen::Vector3f size_min, size_max;
public:
    CSG_Box(Eigen::Vector3f& min,
            Eigen::Vector3f& max,
            Eigen::Transform<float,3,Eigen::Affine> origin_transform = Eigen::Transform<float,3,Eigen::Affine>::Identity());
    virtual void getBoundingBox(float min[3],float max[3])const override;
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior,
                                       NormalsMap& normalsMap)const override;
};
class CSG_Cylinder : public CSG_Geometric {
    Eigen::Vector3f center;
    Eigen::Vector3f bottom_center;
    Eigen::Vector3f top_center;
    Eigen::Vector3f direction;
    float radius;
    float height_min, height_max;
public:
    CSG_Cylinder(Eigen::Vector3f center, Eigen::Vector3f direction, float radius, float height_min, float height_max);
    virtual void getBoundingBox(float min[3],float max[3])const override;
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior,
                                       NormalsMap& normalsMap)const override;
};

#endif // CSG_H