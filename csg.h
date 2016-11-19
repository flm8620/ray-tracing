#ifndef CSG_H
#define CSG_H
#include "intersectable.h"

#include <map>
#include <vector>
#include <memory>
#include "intervals.h"
//typedef std::map<float, Eigen::Vector3f, std::less<float>,
//Eigen::aligned_allocator<std::pair<const float, Eigen::Vector3f> > > NormalsMap;

void EigenToArray(Eigen::Vector3f const & v, float a[3]);

class CSG : public Intersectable{

public:
    virtual ~CSG(){}
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior) const= 0;
    virtual bool rayIntersect(Eigen::Vector3f& rayO,
                              Eigen::Vector3f& rayD,
                              IntersectReport& report) const final;
};

class CSG_Union : public CSG{
    std::vector<std::shared_ptr<CSG> > objects;

public:
    CSG_Union(std::vector<std::shared_ptr<CSG> >& objects);
    virtual Eigen::AlignedBox3f getBoundingBox()const override;
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior) const override;
};

class CSG_Intersection : public CSG{
    std::vector<std::shared_ptr<CSG> > objects;

public:
    CSG_Intersection(std::vector<std::shared_ptr<CSG> >& objects);
    virtual Eigen::AlignedBox3f getBoundingBox()const override;
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior) const override;
};

class CSG_Geometric : public CSG{
};

class CSG_Sphere : public CSG_Geometric {
    Eigen::Vector3f center;
    float radius;
    float sq_radius;
public:
    CSG_Sphere(Eigen::Vector3f center, float radius);
    virtual Eigen::AlignedBox3f getBoundingBox()const override;
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior) const override;
};
class CSG_Box : public CSG_Geometric {
    Eigen::Transform<float,3,Eigen::Affine> origin_transform;
    Eigen::Transform<float,3,Eigen::Affine> origin_transform_inverse;
    Eigen::Vector3f size_min, size_max;
public:
    CSG_Box(Eigen::Vector3f& min,
            Eigen::Vector3f& max,
            Eigen::Transform<float,3,Eigen::Affine> origin_transform = Eigen::Transform<float,3,Eigen::Affine>::Identity());
    virtual Eigen::AlignedBox3f getBoundingBox()const override;
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior)const override;
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
    virtual Eigen::AlignedBox3f getBoundingBox()const override;
    virtual bool rayIntersectIntervals(Eigen::Vector3f& rayO,
                                       Eigen::Vector3f& rayD,
                                       DisjointIntervals& interior)const override;
};

#endif // CSG_H
