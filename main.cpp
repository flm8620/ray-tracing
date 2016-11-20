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
#include <QImage>
#include "camera.h"
#include "scene.h"
#include "render.h"
using namespace std;
using namespace Eigen;


int main()
{
    Scene scene;

    Material m3;
    m3.diffuse_coeff = 1.0;
    m3.specular_coeff = 1.0;
    m3.alpha_phong = 10;
    m3.specular = true;
    m3.transparent = false;
    m3.mirror = false;
    m3.relative_refractive_index = 1.6;
    //scene.addMeshFromPlyFile("/home/leman/Documents/lens2.ply", m3);
    //scene.addMeshFromPlyFile("/home/leman/Documents/lens3.ply", m3);


    Material m;
    m.alpha_phong = 10.0;
    m.diffuse_coeff = 0.8;
    m.specular_coeff = 1.0;
    m.mirror = false;
    m.specular = false;
    m.transparent = false;
    //scene.addMeshFromPlyFile("/home/leman/Documents/bun_zipper2.ply", m);

    std::shared_ptr<CSG> sphere1(new CSG_Sphere(Vector3f(0.0,0.0,0.0), 0.065f));
    std::shared_ptr<CSG> sphere2(new CSG_Sphere(Vector3f(0.0,0.0,0.0), 0.02f));

    float radius = 0.033;
    float length = 0.1;
    std::shared_ptr<CSG> cylinder1(new CSG_Cylinder(Vector3f(0.0,0.0,0.0),
                                                    Vector3f(0.0,0.0,1.0),
                                                    radius,
                                                    -length,
                                                    length));
    std::shared_ptr<CSG> cylinder2(new CSG_Cylinder(Vector3f(0.0,0.0,0.0),
                                                    Vector3f(0.0,1.0,0.0),
                                                    radius,
                                                    -length,
                                                    length));
    std::shared_ptr<CSG> cylinder3(new CSG_Cylinder(Vector3f(0.0,0.0,0.0),
                                                    Vector3f(1.0,0.0,0.0),
                                                    radius,
                                                    -length,
                                                    length));
    std::vector<std::shared_ptr<CSG>> c3 = {cylinder1,cylinder2,cylinder3};
    std::shared_ptr<CSG> cylinders(new CSG_Union(c3));
    std::shared_ptr<CSG> cylinders_inter(new CSG_Intersection(c3));
    Transform<float,3,Eigen::Affine> trans = Transform<float,3,Eigen::Affine>::Identity();
    //trans.translate(Vector3f(0.0,0.0,-0.03));
    float boxsize = 0.05;
    float dx = 0.01;
    std::shared_ptr<CSG> box1(new CSG_Box(Vector3f(-boxsize-dx,-boxsize+dx,-boxsize+dx),
                                          Vector3f( boxsize+dx, boxsize-dx, boxsize-dx),
                                          trans));
    std::shared_ptr<CSG> box2(new CSG_Box(Vector3f(-boxsize+dx,-boxsize-dx,-boxsize+dx),
                                          Vector3f( boxsize-dx, boxsize+dx, boxsize-dx),
                                          trans));
    std::shared_ptr<CSG> box3(new CSG_Box(Vector3f(-boxsize+dx,-boxsize+dx,-boxsize-dx),
                                          Vector3f( boxsize-dx, boxsize-dx, boxsize+dx),
                                          trans));
    std::shared_ptr<CSG> box(new CSG_Box(Vector3f(-boxsize,-boxsize,-boxsize),
                                         Vector3f( boxsize, boxsize, boxsize),
                                         trans));
    std::shared_ptr<CSG> box_union(new CSG_Union({box1,box2,box3}));
    std::shared_ptr<CSG> box_sph(new CSG_Intersection({box,sphere1}));

    std::shared_ptr<CSG> hole(new CSG_Difference(box_sph,cylinders));
    scene.addObject(hole, m3);
    //scene.addObject(cylinders, m3);

    std::shared_ptr<CSG> sphere3(new CSG_Sphere(Vector3f(0.05,0.0,0.0), 0.05f));
    std::shared_ptr<CSG> sphere4(new CSG_Sphere(Vector3f(-0.05,0.0,0.0), 0.05f));
    //scene.addObject(sphere3, m3);
    //scene.addObject(sphere4, m3);
    //scene.exportRTreeToPly("rtree.ply");
    //return 0;

    Material m2;
    m2.diffuse_coeff = 0.3;
    m2.specular_coeff = 0.8;
    m2.specular = false;
    m2.mirror = true;
    //scene.addMeshFromPlyFile("/home/leman/Documents/plan.ply", m2);




    Sunshine sun;
    sun.direction=Vector3f(-0.3,0.1,-1.0);
    sun.intensity=0.8;
    scene.addSunshine(sun);

    sun.direction=Vector3f(1.0,1.0,1.0);
    sun.intensity=0.4;
    scene.addSunshine(sun);

    scene.setAmbientIntensity(0.05);


    Camera cam(800,600,600);
    cam.setPosition(Vector3f(0.08,-0.15,0.15));
    cam.lookAt(Vector3f(0.0,0.0,0.0));
    cam.height=800; cam.width=1000;
    cam.focal=600;

    Render render;
    QImage image = render.renderImage(cam,scene);

    image.save("saved.png");

    return 0;
}

