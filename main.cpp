//
// Test.cpp
//
// This is a direct port of the C version of the RTree test program.
//

#include <iostream>
#include <random>
#include <cmath>
#include <vector>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <Eigen/Geometry>
#include "RTree.h"
#include <QImage>
#include "ply_read.h"
#include "ray_slab.h"

using namespace std;

typedef int ValueType;

int main()
{
    ply_reader::t_ply_result ply_result;
    ply_reader::read_all_in_ply("/home/leman/Documents/bun.ply",ply_result);

    typedef RTree<ValueType, float, 3, float,2> MyTree;
    MyTree tree;

    for(int i=0;i<ply_result.face_index.size();i++){
        float rect_min[3], rect_max[3];
        unsigned int index0 = ply_result.face_index[i][0];
        unsigned int index1 = ply_result.face_index[i][1];
        unsigned int index2 = ply_result.face_index[i][2];
        array<double,3>& v0 = ply_result.vertices[index0];
        array<double,3>& v1 = ply_result.vertices[index1];
        array<double,3>& v2 = ply_result.vertices[index2];
        rect_min[0] = min<double>(v0[0],min<double>(v1[0],v2[0]));
        rect_min[1] = min<double>(v0[1],min<double>(v1[1],v2[1]));
        rect_min[2] = min<double>(v0[2],min<double>(v1[2],v2[2]));

        rect_max[0] = max<double>(v0[0],max<double>(v1[0],v2[0]));
        rect_max[1] = max<double>(v0[1],max<double>(v1[1],v2[1]));
        rect_max[2] = max<double>(v0[2],max<double>(v1[2],v2[2]));
        tree.Insert(rect_min,rect_max,i);
    }
    Eigen::Transform<float,3,Eigen::Affine> E;
    E.translation()<<0.0,0.0,-0.5;



    struct Ray{
        float rayO[3], rayN[3];
    };

    MyTree::intersect_test intersectFun = [](void* userData, MyTree::Rect* rect)->bool {
        Ray* ray_p = static_cast<Ray*>(userData);
        return slab_test_3d<float>(ray_p->rayO,ray_p->rayN,rect->m_min,rect->m_max);
    };

    int H=600,W=800;
    float focal = H;
    float center_x=W/2, center_y=H/2;
    QImage image(H,W,QImage::Format_RGB32);

    for(int i=0;i<H;i++){
        for(int j=0;j<W;j++){
            Eigen::Vector3f v;
            v[0]=j-center_x;
            v[1]=i-center_y;
            v[2]=focal;
            v.normalize();
            //v = E.linear() * v;

            Ray ray;
            ray.rayO[0]=0.0;ray.rayO[1]=0.0;ray.rayO[2]=-0.5;
            ray.rayN[0]=v[0];ray.rayN[1]=v[1];ray.rayN[2]=v[2];
            int count = tree.Search_user_defined(static_cast<void*>(&ray),intersectFun,NULL,NULL);
            if(count>0){
                image.setPixel(i,j,qRgb(255,255,255));
            }
        }
    }


    image.save("saved.png");

    return 0;
}

