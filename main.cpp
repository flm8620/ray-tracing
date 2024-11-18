#include <cmath>
#include <vector>
#include <ctime>

#include <opencv2/opencv.hpp>

#include "camera.h"
#include "scene.h"
#include "render.h"
#include "csg.h"
#include "random_texture.h"

using namespace std;
using namespace Eigen;

int main() {
    Scene scene;

    auto m3 = std::make_shared<Material>();
    m3->diffuse_coeff = 0.5;
    m3->random_diffuse_texture = std::make_shared<RandomNoise>();
    m3->random_diffuse_texture->setScale(5000.0);
    m3->specular_coeff = 0.1;
    m3->alpha_phong = 10;
    m3->specular = true;
    m3->transparent = false;
    m3->mirror = false;
    m3->relative_refractive_index = 1.6;

    auto sphere1 = std::make_shared<CSG_Sphere>(Vector3f(0.0, 0.0, 0.0), 0.065f);

    float radius = 0.033;
    float length = 0.1;
    auto cylinder1 = std::make_shared<CSG_Cylinder>(Vector3f(0.0, 0.0, 0.0),
                                                    Vector3f(0.0, 0.0, 1.0),
                                                    radius,
                                                    -length,
                                                    length);
    auto cylinder2 = std::make_shared<CSG_Cylinder>(Vector3f(0.0, 0.0, 0.0),
                                                    Vector3f(0.0, 1.0, 0.0),
                                                    radius,
                                                    -length,
                                                    length);
    auto cylinder3 = std::make_shared<CSG_Cylinder>(Vector3f(0.0, 0.0, 0.0),
                                                    Vector3f(1.0, 0.0, 0.0),
                                                    radius,
                                                    -length,
                                                    length);
    std::vector<std::shared_ptr<CSG>> c3 = {cylinder1, cylinder2, cylinder3};
    auto cylinders = std::make_shared<CSG_Union>(c3);
    Transform<float, 3, Eigen::Affine> trans = Transform<float, 3, Eigen::Affine>::Identity();
    // trans.translate(Vector3f(0.0,0.0,-0.03));
    float boxsize = 0.05;
    float dx = 0.01;
    auto box = std::make_shared<CSG_Box>(Vector3f(-boxsize, -boxsize, -boxsize),
                                         Vector3f(boxsize, boxsize, boxsize),
                                         trans);
    auto box_sph = std::make_shared<CSG_Intersection>(std::vector<std::shared_ptr<CSG>>{box, sphere1});
    auto hole = std::make_shared<CSG_Difference>(box_sph, cylinders);
    scene.addObject(hole, m3);
    // scene.addObject(cylinders, m3);

    // auto sphere3 = std::make_shared<CSG_Sphere>(Vector3f(0.05, 0.0, 0.0), 0.05f);
    // auto sphere4 = std::make_shared<CSG_Sphere>(Vector3f(-0.05, 0.0, 0.0), 0.05f);

    // scene.addObject(sphere3, m3);
    // scene.addObject(sphere4, m3);
    // exportRTreeToPly(scene.getTree(), "rtree.ply");
    // return 0;

    // Material m2;
    // m2.diffuse_coeff = 0.3;
    // m2.specular_coeff = 0.8;
    // m2.specular = false;
    // m2.mirror = true;
    // scene.addMeshFromPlyFile("/home/leman/Documents/plan.ply", m2);


    Sunshine sun;
    sun.direction = Vector3f(-0.3, 0.1, -1.0);
    sun.intensity = 0.8;
    scene.addSunshine(sun);

    sun.direction = Vector3f(1.0, 1.0, 1.0);
    sun.intensity = 0.4;
    scene.addSunshine(sun);

    scene.setAmbientIntensity(0.8);


    Camera cam(800, 600, 600);
    cam.setPosition(Vector3f(0.08, -0.15, 0.15));
    cam.lookAt(Vector3f(0.0, 0.0, 0.0));
    cam.height = 800;
    cam.width = 1000;
    cam.focal = 600;

    Render render;
    cv::Mat image = render.renderImage(cam, scene);

    cv::imwrite("saved.png", image);

    return 0;
}
