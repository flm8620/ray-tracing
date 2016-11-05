#include "scene.h"
#include <algorithm>
#include "ray_slab.h"
#include "geometry_util.h"
#include <iostream>
using namespace std;
using namespace Eigen;
Scene::Scene()
{

}

void Scene::readPlyFile(const char *file)
{
    ply_reader::t_ply_result ply_result;
    ply_reader::read_all_in_ply(file,ply_result);

    face_index = ply_result.face_index;
    vertices.resize(ply_result.vertices.size());

    int i=0;
    for(auto &v : ply_result.vertices){
        vertices[i][0] = v[0];
        vertices[i][1] = v[1];
        vertices[i][2] = v[2];
        i++;
    }

    tree.RemoveAll();


    for(faceID i=0;i<face_index.size();i++){
        float rect_min[3], rect_max[3];
        unsigned int index0 = face_index[i][0];
        unsigned int index1 = face_index[i][1];
        unsigned int index2 = face_index[i][2];
        Vector3f& v0 = vertices[index0];
        Vector3f& v1 = vertices[index1];
        Vector3f& v2 = vertices[index2];
        rect_min[0] = min(v0[0],min(v1[0],v2[0]));
        rect_min[1] = min(v0[1],min(v1[1],v2[1]));
        rect_min[2] = min(v0[2],min(v1[2],v2[2]));

        rect_max[0] = max(v0[0],max(v1[0],v2[0]));
        rect_max[1] = max(v0[1],max(v1[1],v2[1]));
        rect_max[2] = max(v0[2],max(v1[2],v2[2]));
        tree.Insert(rect_min,rect_max,i);
    }
}

bool Scene::ray_intersect_query(Ray ray_, IntersectReport *report) const
{
    struct RaySimple{
        float O[3], D[3];
    } ray;
    ray.O[0]=ray_.O[0];
    ray.O[1]=ray_.O[1];
    ray.O[2]=ray_.O[2];
    ray.D[0]=ray_.D[0];
    ray.D[1]=ray_.D[1];
    ray.D[2]=ray_.D[2];

    MyTree::intersect_test intersectFun = [](void* userData, MyTree::Rect* rect)->bool {
        RaySimple* p = static_cast<RaySimple*>(userData);
        return slab_test_3d<float>(p->O, p->D, rect->m_min, rect->m_max);
    };

    vector<faceID> suspects;
    MyTree::t_resultCallback suspect_callback =
            [](faceID value,void* suspects)->bool {
        vector<faceID>* p = static_cast<vector<faceID>*>(suspects);
        p->push_back(value);
        return true;
    };

    int count = tree.Search_user_defined(static_cast<void*>(&ray),intersectFun,suspect_callback,&suspects);
    if(count>0){
        Eigen::Vector3f v1,v2,v3,O,D;
        O[0]=ray.O[0];O[1]=ray.O[1];O[2]=ray.O[2];
        D[0]=ray.D[0];D[1]=ray.D[1];D[2]=ray.D[2];
        D.normalize();
        float distance = numeric_limits<float>::max();
        faceID id = -1;
        Vector3f intersect_point;
        Vector3f closest_intersect_point;
        for(faceID i : suspects) {
            v1 = vertices[face_index[i][0]];
            v2 = vertices[face_index[i][1]];
            v3 = vertices[face_index[i][2]];
            float d;

            if(ray_triangle_intersect(v1,v2,v3,ray_.O,ray_.D,&d,&intersect_point)){
                if(d < distance){
                    id = i;
                    closest_intersect_point=intersect_point;
                }
                distance = std::min(distance,d);
            }
        }
        if(id!=-1){
            report->face.v0=vertices[face_index[id][0]];
            report->face.v1=vertices[face_index[id][1]];
            report->face.v2=vertices[face_index[id][2]];
            Vector3f a=report->face.v1 - report->face.v0;
            Vector3f b=report->face.v2 - report->face.v0;
            Vector3f n = a.cross(b);
            n.normalize();
            if(n.dot(ray_.D) > 0) n=-n;
            report->normal = n;
            report->intersect_point = closest_intersect_point;
            report->faceid = id;
            return true;
        }
    }

    return false;
}

void Scene::addSunshine(Sunshine s)
{
    s.direction.normalize();
    sunshines.push_back(s);
}
