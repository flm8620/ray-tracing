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
    Material m;
    m.alpha_phong = 10.0;
    m.diffuse_coeff = 0.2;
    m.specular_coeff = 1.0;
    scene.readPlyFile("/home/leman/Documents/bun_zipper2.ply", m);
    Sunshine sun;
    sun.direction=Eigen::Vector3f(0.0,0.0,-1.0);
    sun.intensity=1.0;
    scene.addSunshine(sun);

    sun.direction=Eigen::Vector3f(1.0,1.0,1.0);
    sun.intensity=0.5;
    scene.addSunshine(sun);

    scene.setAmbientIntensity(0.05);


    Camera cam;
    cam.setPosition(Eigen::Vector3f(0.0,-0.2,0.1));
    cam.lookAt(Eigen::Vector3f(0.0,0.0,0.03));
    cam.height=800; cam.width=1000;
    cam.focal=800;

    Render render;
    QImage image = render.renderImage(cam,scene);

    image.save("saved.png");

    return 0;
}

