#include "csg.h"
#include "ray_slab.h"
CSG_Sphere::CSG_Sphere(Eigen::Vector3f center, float radius)
{
    this->center = center;
    this->radius = radius;
    this->sq_radius = radius * radius;
}

void CSG_Sphere::getBoundingBox(float min[3], float max[3])const
{
    min[0] = center[0] - radius;
    min[1] = center[1] - radius;
    min[2] = center[2] - radius;
    max[0] = center[0] + radius;
    max[1] = center[1] + radius;
    max[2] = center[2] + radius;
}

bool CSG_Sphere::rayIntersectIntervals(Eigen::Vector3f &rayO, Eigen::Vector3f &rayD, DisjointIntervals &interior, NormalsMap &normalsMap)const
{
    float t = rayD.dot(center-rayO);
    Eigen::Vector3f P = rayO + t * rayD;
    Eigen::Vector3f CP = P - center;
    float sqd = CP.squaredNorm();
    if(sqd >= sq_radius) return false;
    float dt = std::sqrt(sq_radius - sqd);
    float t_in = dt - t;
    float t_out = dt + t;
    if(t_out <= 0) return false;
    Eigen::Vector3f n_in = (rayD * t_in + rayO) - rayO;
    n_in.normalize();
    Eigen::Vector3f n_out = (rayD * t_out + rayO) - rayO;
    n_out.normalize();
    if(t_in<0) t_in = DisjointIntervals::NEG_INF;
    normalsMap[t_in]=n_in;
    normalsMap[t_out]=n_out;
    interior = DisjointIntervals({{t_in,t_out}});
    return true;
}

CSG_Box::CSG_Box(Eigen::Vector3f &min, Eigen::Vector3f &max, Eigen::Transform<float, 3, Eigen::Affine> origin_transform)
{
    this->size_min = min;
    this->size_max = max;
    this->origin_transform = origin_transform;
    this->origin_transform_inverse = origin_transform.inverse(Eigen::Affine);
}

void CSG_Box::getBoundingBox(float min[3], float max[3])const
{
    for(auto i : {0,1,2}){
        min[i]=std::numeric_limits<float>::max();
        max[i]=std::numeric_limits<float>::min();
    }
    for(auto & v0 : {size_min, size_max} ){
        for(auto & v1 : {size_min, size_max} ){
            for(auto & v2 : {size_min, size_max} ){
                Eigen::Vector3f v(v0[0],v1[1],v2[2]);
                v = origin_transform.linear() * v;
                for(auto i : {0,1,2}){
                    min[i] = std::min(min[i],v[i]);
                    max[i] = std::max(max[i],v[i]);
                }
            }
        }
    }
}

bool CSG_Box::rayIntersectIntervals(Eigen::Vector3f &rayO, Eigen::Vector3f &rayD, DisjointIntervals &interior, NormalsMap &normalsMap)const
{
    Eigen::Vector3f rayO_t = origin_transform_inverse * rayO;
    Eigen::Vector3f rayD_t = origin_transform_inverse.linear() * rayD;
    float rayO_a[3], rayD_a[3], boxMin[3], boxMax[3];
    EigenToArray(rayO_t,rayO_a);
    EigenToArray(rayD_t,rayO_a);
    EigenToArray(size_min,boxMin);
    EigenToArray(size_max,boxMax);
    int axis_in, axis_out;
    float t_in, t_out;
    if(slab_test_3d_info<float>(rayO_a, rayD_a, boxMin, boxMax, t_in, t_out, axis_in, axis_out)){
        if(t_in<0) t_in = DisjointIntervals::NEG_INF;
        interior = DisjointIntervals({{t_in, t_out}});
        Eigen::Vector3f n_in, n_out;
        switch (axis_in) {
        case 0:
            n_in = Eigen::Vector3f::UnitX();
            break;
        case 1:
            n_in = Eigen::Vector3f::UnitY();
            break;
        case 2:
            n_in = Eigen::Vector3f::UnitZ();
            break;
        }
        switch (axis_out) {
        case 0:
            n_out = Eigen::Vector3f::UnitX();
            break;
        case 1:
            n_out = Eigen::Vector3f::UnitY();
            break;
        case 2:
            n_out = Eigen::Vector3f::UnitZ();
            break;
        }
        if(n_in.dot(rayD_t)>0.0f)n_in=-n_in;
        if(n_out.dot(rayD_t)<0.0f)n_out=-n_out;
        normalsMap[t_in]=origin_transform.linear() * n_in;
        normalsMap[t_out]=origin_transform.linear() * n_out;
    }else{
        return false;
    }


}

CSG_Cylinder::CSG_Cylinder(Eigen::Vector3f center, Eigen::Vector3f direction, float radius, float height_min, float height_max):center(center),direction(direction.normalized()),radius(radius), height_min(height_min), height_max(height_max)
{
    top_center = this->center + this->direction * height_max;
    bottom_center = this->center + this->direction * height_min;

}

void CSG_Cylinder::getBoundingBox(float min[3], float max[3])const
{
    min[0]=std::min(top_center[0]-radius, bottom_center[0]-radius);
    min[1]=std::min(top_center[0]-radius, bottom_center[0]-radius);
    min[2]=std::min(top_center[0]-radius, bottom_center[0]-radius);
    max[0]=std::max(top_center[0]+radius, bottom_center[0]+radius);
    max[1]=std::max(top_center[0]+radius, bottom_center[0]+radius);
    max[2]=std::max(top_center[0]+radius, bottom_center[0]+radius);
}

bool CSG_Cylinder::rayIntersectIntervals(Eigen::Vector3f &rayO, Eigen::Vector3f &rayD, DisjointIntervals &interior, NormalsMap &normalsMap)const
{
    Eigen::Vector3f dp = rayO - center;
    float A = (rayD - rayD.dot(direction) * direction).squaredNorm();
    float B = 2 * (rayD-rayD.dot(direction)* direction).dot(dp - dp.dot(direction)*direction);
    float C = (dp - dp.dot(direction)*direction).squaredNorm() - radius * radius;
    float det = B*B - 4*A*C;
    if(det <= 0.0f) return false;
    det = std::sqrt(det);
    float t_in_wall = (-B - det) / (2*A);
    float t_out_wall = (-B + det) / (2*A);

    Eigen::Vector3f a = bottom_center - rayO;
    Eigen::Vector3f b = top_center - rayO;
    float t_in_cover, t_out_cover;
    if(std::abs(rayD.dot(direction)) < 1e-5f){
        if(a.dot(direction) > 0.0f && b.dot(direction) < 0.0f){
            t_in_cover=std::numeric_limits<float>::min();
            t_out_cover=std::numeric_limits<float>::max();
        }else{
            return false;
        }
    }else{
        t_in_cover = a.dot(direction) / rayD.dot(direction);
        t_out_cover = b.dot(direction) / rayD.dot(direction);
        if(t_in_cover > t_out_cover) std::swap(t_in_cover,t_out_cover);
    }

    float t_in = std::max(t_in_cover, t_in_wall);
    float t_out = std::min(t_out_cover, t_out_wall);
    if(t_in > t_out) return false;

    Eigen::Vector3f p_in = rayO + t_in * rayD;
    Eigen::Vector3f p_out = rayO + t_out * rayD;
    Eigen::Vector3f n_in;
    Eigen::Vector3f n_out;
    if(t_in_wall < t_in_cover){
        if(rayD.dot(direction) > 0.0f )
            n_in= -1 * direction;
        else
            n_in = direction;
    }else{
        n_in= p_in - p_in.dot(direction)*direction;
        n_in.normalize();
    }
    if(t_out_wall > t_out_cover){
        if(rayD.dot(direction) < 0.0f )
            n_out= -1 * direction;
        else
            n_out = direction;
    }else{
        n_out = p_out - p_out.dot(direction)*direction;
        n_out.normalize();
    }

    if(t_in <= 0.0f) t_in=DisjointIntervals::NEG_INF;
    interior = DisjointIntervals({{t_in, t_out}});
    normalsMap[t_in] = n_in;
    normalsMap[t_out] = n_out;
    return true;
}

void EigenToArray(Eigen::Vector3f &v, float a[3])
{
    a[0]=v[0]; a[1]=v[1]; a[2]=v[2];
}

bool CSG_Union::rayIntersectIntervals(Eigen::Vector3f &rayO, Eigen::Vector3f &rayD, DisjointIntervals &interior, NormalsMap &normalsMap) const
{
    bool r = false;
    for(auto obj : objects){
        NormalsMap normalsMap_;
        DisjointIntervals interior_;
        if(obj->rayIntersectIntervals(rayO, rayD, interior_, normalsMap_)){
            normalsMap.insert(normalsMap_.begin(), normalsMap_.end());
            interior.unionWith(interior_);
            r = true;
        }
    }
    return r;
}

void CSG_Union::getBoundingBox(float min[3], float max[3])const
{
    min[0]=min[1]=min[2]=std::numeric_limits<float>::max();
    max[0]=max[1]=max[2]=std::numeric_limits<float>::min();
    for(auto obj : objects){
        float min_[3]; float max_[3];
        obj->getBoundingBox(min, max);
        min[0]=std::min(min[0],min_[0]);
        min[1]=std::min(min[1],min_[1]);
        min[2]=std::min(min[2],min_[2]);
        max[0]=std::max(max[0],max_[0]);
        max[1]=std::max(max[1],max_[1]);
        max[2]=std::max(max[2],max_[2]);
    }
}

CSG_Union::CSG_Union(std::vector<std::shared_ptr<CSG> > &objects)
    :objects(objects)
{

}

bool CSG::rayIntersect(Eigen::Vector3f &rayO,
                       Eigen::Vector3f &rayD,
                       IntersectReport& report) const
{
    DisjointIntervals interior;
    NormalsMap normalMaps;
    bool r = rayIntersectIntervals(rayO,rayD,interior,normalMaps);
    if(!r) return false;
    auto end = interior.end();
    report.t=0.0;
    for(auto it = interior.begin();it!=end;it++){
        if(it->left > 0.0f){
            report.t = it->left;
            break;
        }
        if(it->right > 0.0f){
            report.t = it->right;
            break;
        }
    }
    if(report.t==0.0) throw std::logic_error("t should be positive here!");
    auto it = normalMaps.find(report.t);
    if(it == normalMaps.end()) throw std::logic_error("should find a normal here!");
    report.normal = it->second;
    report.intersect_point = rayO + rayD * report.t;
    return true;
}
