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
    m3.diffuse_coeff = 0.0;
    m3.specular_coeff = 1.0;
    m3.specular = false;
    m3.transparent = true;
    m3.mirror = true;
    m3.relative_refractive_index = 1.1;
    //scene.addMeshFromPlyFile("/home/leman/Documents/lens2.ply", m3);
    //scene.addMeshFromPlyFile("/home/leman/Documents/lens3.ply", m3);


    Material m;
    m.alpha_phong = 10.0;
    m.diffuse_coeff = 0.8;
    m.specular_coeff = 1.0;
    m.mirror = false;
    m.specular = false;
    m.transparent = false;
    scene.addMeshFromPlyFile("/home/leman/Documents/bun_zipper2.ply", m);

    std::shared_ptr<CSG> sphere1(new CSG_Sphere(Vector3f(-0.03,-0.05,0.04), 0.02f));
    std::shared_ptr<CSG> sphere2(new CSG_Sphere(Vector3f(-0.03,-0.08,0.04), 0.02f));
    std::vector<std::shared_ptr<CSG>> spheres = {sphere1, sphere2};
    std::shared_ptr<CSG> lens(new CSG_Intersection(spheres));

    std::shared_ptr<CSG> cylinder(new CSG_Cylinder(Vector3f(-0.03,-0.05,0.04),
                                                   Vector3f(1.0,0.0,1.0),
                                                   0.02,
                                                   0.0,
                                                   0.03));
    std::shared_ptr<CSG> box(new CSG_Box(Vector3f(-0.03,-0.05,0.04),
                                         Vector3f(-0.02,-0.04,0.05)));
    scene.addObject(box, m);


    //scene.exportRTreeToPly("rtree.ply");
    //return 0;

    Material m2;
    m2.diffuse_coeff = 0.3;
    m2.specular_coeff = 0.8;
    m2.specular = false;
    m2.mirror = true;
    scene.addMeshFromPlyFile("/home/leman/Documents/plan.ply", m2);




    Sunshine sun;
    sun.direction=Vector3f(0.0,0.0,-1.0);
    sun.intensity=0.8;
    scene.addSunshine(sun);

    sun.direction=Vector3f(1.0,1.0,1.0);
    sun.intensity=0.2;
    //scene.addSunshine(sun);

    scene.setAmbientIntensity(0.05);


    Camera cam(800,600,600);
    cam.setPosition(Vector3f(0.0,-0.20,0.06));
    cam.lookAt(Vector3f(-0.05,0.0,0.03));
    cam.height=800; cam.width=1000;
    cam.focal=600;

    Render render;
    QImage image = render.renderImage(cam,scene);

    image.save("saved.png");

    return 0;
}

