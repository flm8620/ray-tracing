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
    scene.setBackground(Eigen::Vector3f::Zero());

    auto m3 = std::make_shared<Material>();
    m3->diffuse_color = Eigen::Vector3f(0.5, 0.5, 0.5);
    m3->random_diffuse_texture = std::make_shared<RandomTexture>();
    m3->random_diffuse_texture->setScale(5000.0);
    m3->specular_coeff = Eigen::Vector3f(0.1, 0.1, 0.1);
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

    {
        auto m_frame = std::make_shared<Material>();
        m_frame->diffuse_color = Eigen::Vector3f(0.5, 0.5, 0.5);
        m_frame->random_diffuse_texture = std::make_shared<RandomTexture>();
        m_frame->random_diffuse_texture->setScale(2000.0);
        m_frame->specular_coeff = Eigen::Vector3f(0.1, 0.1, 0.1);
        float frame_size = 0.07;
        float margin = 0.01;
        float large_margin = 0.02;

        auto inner_box = std::make_shared<CSG_Box>(Vector3f(-frame_size, -frame_size, -frame_size),
                                                   Vector3f(frame_size, frame_size, frame_size));
        float outer_size = frame_size + margin;
        auto outer_box = std::make_shared<CSG_Box>(Vector3f(-outer_size, -outer_size, -outer_size),
                                                   Vector3f(outer_size, outer_size, outer_size));
        auto inner_box_x = std::make_shared<CSG_Box>(Vector3f(-frame_size - large_margin, -frame_size, -frame_size),
                                                     Vector3f(frame_size + large_margin, frame_size, frame_size));
        auto inner_box_y = std::make_shared<CSG_Box>(Vector3f(-frame_size, -frame_size - large_margin, -frame_size),
                                                     Vector3f(frame_size, frame_size + large_margin, frame_size));
        auto inner_box_z = std::make_shared<CSG_Box>(Vector3f(-frame_size, -frame_size, -frame_size - large_margin),
                                                     Vector3f(frame_size, frame_size, frame_size + large_margin));
        auto inner_hole = std::make_shared<CSG_Union>(std::vector<std::shared_ptr<CSG>>{inner_box_x, inner_box_y, inner_box_z});
        auto frame = std::make_shared<CSG_Difference>(outer_box, inner_hole);
        scene.addObject(frame, m_frame);

        auto m_fog = std::make_shared<Material>();
        m_fog->is_fog = true;
        m_fog->fog_sigma = 10.0;
        m_fog->fog_color = Eigen::Vector3f(1.0, 1.0, 1.0);
        auto frame_fog = std::make_shared<CSG_Difference>(outer_box, inner_box);
        scene.addObject(frame_fog, m_fog);
    }
    if (false) {
        auto m_fog = std::make_shared<Material>();
        m_fog->is_fog = true;
        m_fog->fog_sigma = 10.0;
        m_fog->fog_color = Eigen::Vector3f(1.0, 1.0, 1.0);
        auto box_fog = std::make_shared<CSG_Box>(Vector3f(0.2, -10, -10),
                                                 Vector3f(0.22, 10, 10),
                                                 trans);
        auto sphere_fog = std::make_shared<CSG_Sphere>(Vector3f(0.0, 0.0, 0.0), 0.06f);
        scene.addObject(box_fog, m_fog);
    }



    Sunshine sun1;
    sun1.direction = Vector3f(-0.3, 0.1, -1.0);
    sun1.color = Eigen::Vector3f(0.8, 0.8, 0.8);
    scene.addSunshine(sun1);

    Sunshine sun2;
    sun2.direction = Vector3f(1.0, 1.0, 1.0);
    sun2.color = Eigen::Vector3f(0.4, 0.4, 0.4);
    scene.addSunshine(sun2);

    scene.setAmbientColor(Eigen::Vector3f(0.4, 0.4, 0.4));


    Camera cam(800, 600, 600);

    Render render;
    const int N = 10;
    for (int i = 0; i < 1; i++) {
        const double angle = 3.1415926 * i / N * 0.5;
        // const double angle = 3.1415 * 0.2;
        const double radius = 0.3;
        const double x = std::cos(angle) * radius;
        const double y = std::sin(angle) * radius;
        const double z = 0.0;
        cam.setPosition(Vector3f(x, y, z));
        cam.lookAt(Vector3f(0.0, 0.0, 0.0));
        cv::Mat image = render.renderImage(cam, scene);
        cv::imwrite("saved-" + std::to_string(i) + ".png", image);
    }


    return 0;
}
