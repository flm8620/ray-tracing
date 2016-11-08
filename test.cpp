#include <iostream>

#include <random>
#include <cmath>
#include <vector>
#include <ctime>
#include <chrono>
#include "gtest/gtest.h"
#include "ray_slab.h"
#include "RTree.h"

TEST(slab_test,test1){
    float boxMin[3]={0.0,0.0,0.0};
    float boxMax[3]={1.0,1.0,1.0};
    float rayO[3]={0.5,0.5,0.5};
    float rayN[3]={1.0,0.0,0.0};
    bool result=slab_test_3d(rayO,rayN,boxMin,boxMax);
    EXPECT_EQ(true,result);

    float rayN2[3]={0.577,0.577,0.577};
    result=slab_test_3d(rayO,rayN2,boxMin,boxMax);
    EXPECT_EQ(true,result);

    float rayN3[3]={-0.577,0.577,0.577};
    result=slab_test_3d(rayO,rayN3,boxMin,boxMax);
    EXPECT_EQ(true,result);

    float rayO2[3]={-0.5,0.5,0.5};
    result=slab_test_3d(rayO2,rayN,boxMin,boxMax);
    EXPECT_EQ(true,result);

    float rayO3[3]={0.5,-0.5,0.5};
    result=slab_test_3d(rayO3,rayN,boxMin,boxMax);
    EXPECT_EQ(false,result);
}

TEST(slab_test,test_with_rtree){
    typedef int ValueType;
    typedef RTree<ValueType, float, 3, float, 2> MyTree;
    MyTree tree;
    float data[]={
        0.0,0.0,0.0,
        1.0,1.0,1.0,

        2.0,0.0,0.0,
        3.0,1.0,1.0,

        0.0,2.0,0.0,
        1.0,3.0,1.0,

        0.0,0.0,2.0,
        1.0,1.0,3.0,
    };
    const int nrect = sizeof(data)/sizeof(data[0])/2/3;
    float rectmin[nrect][3],rectmax[nrect][3];
    for(int i=0; i<nrect;i++){
        for(int j=0;j<3;j++){
            rectmin[i][j]=data[i*6+j];
            rectmax[i][j]=data[i*6+3+j];
        }
        tree.Insert(rectmin[i],rectmax[i],i);
    }

    struct Ray{
        float rayO[3], rayN[3];
    };

    Ray ray={{0.5,0.5,0.5},{1.0,0.0,0.0}};
    MyTree::intersect_test intersectFun = [](void* userData, MyTree::Rect* rect)->bool {
        Ray* ray_p = static_cast<Ray*>(userData);
        return slab_test_3d<float>(ray_p->rayO,ray_p->rayN,rect->m_min,rect->m_max);
    };
    int count = tree.Search_user_defined(static_cast<void*>(&ray),intersectFun,NULL,NULL);
    EXPECT_EQ(2,count);

}

TEST(RTree,rtree){
    using namespace std;

    struct Rect3d{
        float min[3],max[3];
    };

    typedef int ValueType;


    typedef RTree<ValueType, float, 3, float,2> MyTree;
    MyTree tree;
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine gen(seed1);
    std::uniform_real_distribution<float> random_exp(-1.0,0.5);
    std::uniform_real_distribution<float> random_01(0.0,1.0);

    size_t i,nrects;
    nrects=100;
    cout << "nrects = " << nrects << "\n";
    float x_size, y_size, z_size;
    x_size=y_size=z_size=10.0;

    vector<Rect3d> rects(nrects);
    for(i=0; i<nrects; i++)
    {
        float dx,dy,dz;
        dx=pow(10.0,random_exp(gen));
        dy=pow(10.0,random_exp(gen));
        dz=pow(10.0,random_exp(gen));
        //if(i<10) dx=dy=dx=9;
        float x,y,z;
        x = random_01(gen) * (x_size-dx);
        y = random_01(gen) * (y_size-dy);
        z = random_01(gen) * (z_size-dz);
        rects[i].min[0]=x; rects[i].min[1]=y; rects[i].min[2]=z;
        rects[i].max[0]=x+dx; rects[i].max[1]=y+dy; rects[i].max[2]=z+dz;

    }
    cout<<"Generated "<<nrects<<" rects"<<endl;

    clock_t start_clock;
    start_clock = clock();

    for(i=0; i<nrects; i++)
        tree.Insert(rects[i].min, rects[i].max, i); // Note, all values including zero are fine in this version

    double past = (double)(clock()-start_clock)/CLOCKS_PER_SEC;
    cout << "Insert took "<< past <<" second(s)."<< endl;
    cout << "Mean time: "<< past/nrects <<" second(s)."<< endl;


    // generate search rects
    size_t nsearch=100;
    rects.resize(nsearch);
    for(i=0; i<nsearch; i++)
    {
        float dx,dy,dz;
        dx=pow(10.0,random_exp(gen));
        dy=pow(10.0,random_exp(gen));
        dz=pow(10.0,random_exp(gen));
        float x,y,z;
        x = random_01(gen) * (x_size-dx);
        y = random_01(gen) * (y_size-dy);
        z = random_01(gen) * (z_size-dz);
        rects[i].min[0]=x; rects[i].min[1]=y; rects[i].min[2]=z;
        rects[i].max[0]=x+dx; rects[i].max[1]=y+dy; rects[i].max[2]=z+dz;
    }

    start_clock = clock();

    int hits=0;
    for(i=0; i<nsearch; i++)
        hits+=tree.Search(rects[i].min, rects[i].max, NULL, NULL);
    past = (double)(clock()-start_clock)/CLOCKS_PER_SEC;
    cout << "Search took "<< past <<" second(s)."<< endl;

    cout << "Mean time: "<< past/nsearch <<" second(s)."<< endl;
    cout << "mean hits: "<< double(hits)/nsearch << endl;

    // Iterator test
    int itIndex = 0;
    MyTree::Iterator it;
    for( tree.GetFirst(it);
         !tree.IsNull(it);
         tree.GetNext(it) )
    {
      int value = tree.GetAt(it);

      float boundsMin[3] = {0,0,0};
      float boundsMax[3] = {0,0,0};
      it.GetBounds(boundsMin, boundsMax);
      cout << "it[" << itIndex++ << "] " << value << " = (" << boundsMin[0] << "," << boundsMin[1] << "," << boundsMax[0] << "," << boundsMax[1] << ")\n";
    }

    // Iterator test, alternate syntax
    itIndex = 0;
    tree.GetFirst(it);
    while( !it.IsNull() )
    {
      int value = *it;
      ++it;
      cout << "it[" << itIndex++ << "] " << value << "\n";
    }

}

