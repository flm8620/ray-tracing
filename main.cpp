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
    scene.readPlyFile("/home/leman/Documents/bun_zipper2.ply");
    Sunshine sun;
    sun.direction=Eigen::Vector3f(0.0,0.0,-1.0);
    sun.intensity=0.9;
    scene.addSunshine(sun);

    sun.direction=Eigen::Vector3f(1.0,0.0,1.0);
    sun.intensity=0.2;
    scene.addSunshine(sun);


    Camera cam;
    cam.setPosition(Eigen::Vector3f(0.0,-0.4,0.2));
    cam.lookAt(Eigen::Vector3f(0.0,0.0,0.0));
    cam.height=2400; cam.width=4000;
    cam.focal=2400;

    Render render;
    QImage image = render.renderImage(cam,scene);

    image.save("saved.png");

    return 0;
}

