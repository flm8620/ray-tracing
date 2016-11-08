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


int main()
{
    Scene scene;

    Material m3;
    m3.diffuse_coeff = 0.0;
    m3.specular_coeff = 1.0;
    m3.specular = false;
    m3.transparent = true;
    m3.mirror = true;
    m3.relative_refractive_index = 1.6;
    scene.readPlyFile("/home/leman/Documents/lens2.ply", m3);


    Material m;
    m.alpha_phong = 10.0;
    m.diffuse_coeff = 0.8;
    m.specular_coeff = 1.0;
    m.mirror = false;
    m.specular = false;
    m.transparent = false;
    scene.readPlyFile("/home/leman/Documents/bun_zipper2.ply", m);



    //scene.exportRTreeToPly("rtree.ply");
    //return 0;

    Material m2;
    m2.diffuse_coeff = 0.3;
    m2.specular_coeff = 0.8;
    m2.specular = false;
    m2.mirror = true;
    scene.readPlyFile("/home/leman/Documents/plan.ply", m2);




    Sunshine sun;
    sun.direction=Eigen::Vector3f(0.0,0.0,-1.0);
    sun.intensity=0.8;
    scene.addSunshine(sun);

    sun.direction=Eigen::Vector3f(1.0,1.0,1.0);
    sun.intensity=0.2;
    scene.addSunshine(sun);

    scene.setAmbientIntensity(0.05);


    Camera cam(800,600,600);
    cam.setPosition(Eigen::Vector3f(0.0,-0.20,0.06));
    cam.lookAt(Eigen::Vector3f(-0.05,0.0,0.03));
    cam.height=800; cam.width=1000;
    cam.focal=600;

    Render render;
    QImage image = render.renderImage(cam,scene);

    image.save("saved.png");

    return 0;
}

