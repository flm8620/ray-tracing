#include <cmath>
#include <random>
#include <vector>
#include <ctime>

#include <opencv2/opencv.hpp>

#include "camera.h"
#include "scene.h"
#include "render.h"
#include "csg.h"
#include "random_texture.h"
#include "colmap_convert.h"

void example_cube_rgb() {
    const std::filesystem::path output_dir = "cube_rgb_output";
    std::filesystem::create_directories(output_dir);

    Scene scene;
    scene.setBackground(Eigen::Vector3f::Zero());

    Sunshine sun1;
    sun1.direction = Eigen::Vector3f(-0.3, 0.1, -1.0);
    sun1.color = Eigen::Vector3f(0.8, 0.8, 0.8);
    scene.addSunshine(sun1);

    Sunshine sun2;
    sun2.direction = Eigen::Vector3f(-1.0, -1.0, -1.0);
    sun2.color = Eigen::Vector3f(0.4, 0.4, 0.4);
    scene.addSunshine(sun2);

    scene.setAmbientColor(Eigen::Vector3f(0.4, 0.4, 0.4));

    int width = 512;
    int height = 512;
    float focal = 512.;
    Camera cam(width, height, focal);
    const double cam_radius = 2.0;

    Render render;

    {
        auto m = std::make_shared<Material>();
        m->diffuse_color = Eigen::Vector3f(1.0, 0.0, 0.0);
        m->specular = false;
        Eigen::Translation3f t(0.5, 0.0, 0.0);
        Eigen::Affine3f ta = Eigen::Affine3f(t);
        const float size = 0.2;
        auto box = std::make_shared<CSG_Box>(
            Eigen::Vector3f(-size, -size, -size),
            Eigen::Vector3f(size, size, size),
            ta);
        scene.addObject(box, m);
    }
    {
        auto m = std::make_shared<Material>();
        m->diffuse_color = Eigen::Vector3f(1.0, 0.5, 0.5);
        m->specular = false;
        Eigen::Translation3f t(-0.5, 0.0, 0.0);
        Eigen::Affine3f ta = Eigen::Affine3f(t);
        const float size = 0.2;
        auto box = std::make_shared<CSG_Box>(
            Eigen::Vector3f(-size, -size, -size),
            Eigen::Vector3f(size, size, size),
            ta);
        scene.addObject(box, m);
    }
    {
        auto m = std::make_shared<Material>();
        m->diffuse_color = Eigen::Vector3f(0.0, 1.0, 0.0);
        m->specular = false;
        Eigen::Translation3f t(0.0, 0.5, 0.0);
        Eigen::Affine3f ta = Eigen::Affine3f(t);
        const float size = 0.2;
        auto box = std::make_shared<CSG_Box>(
            Eigen::Vector3f(-size, -size, -size),
            Eigen::Vector3f(size, size, size),
            ta);
        scene.addObject(box, m);
    }
    {
        auto m = std::make_shared<Material>();
        m->diffuse_color = Eigen::Vector3f(0.5, 1.0, 0.5);
        m->specular = false;
        Eigen::Translation3f t(0.0, -0.5, 0.0);
        Eigen::Affine3f ta = Eigen::Affine3f(t);
        const float size = 0.2;
        auto box = std::make_shared<CSG_Box>(
            Eigen::Vector3f(-size, -size, -size),
            Eigen::Vector3f(size, size, size),
            ta);
        scene.addObject(box, m);
    }
    {
        auto m = std::make_shared<Material>();
        m->diffuse_color = Eigen::Vector3f(0.0, 0.0, 1.0);
        m->specular = false;
        Eigen::Translation3f t(0.0, 0.0, 0.5);
        Eigen::Affine3f ta = Eigen::Affine3f(t);
        const float size = 0.2;
        auto box = std::make_shared<CSG_Box>(
            Eigen::Vector3f(-size, -size, -size),
            Eigen::Vector3f(size, size, size),
            ta);
        scene.addObject(box, m);
    }
    {
        auto m = std::make_shared<Material>();
        m->diffuse_color = Eigen::Vector3f(0.5, 0.5, 1.0);
        m->specular = false;
        Eigen::Translation3f t(0.0, 0.0, -0.5);
        Eigen::Affine3f ta = Eigen::Affine3f(t);
        const float size = 0.2;
        auto box = std::make_shared<CSG_Box>(
            Eigen::Vector3f(-size, -size, -size),
            Eigen::Vector3f(size, size, size),
            ta);
        scene.addObject(box, m);
    }

    {
        auto m = std::make_shared<Material>();
        m->diffuse_color = Eigen::Vector3f(0.5, 0.5, 0.5);
        m->random_diffuse_texture = std::make_shared<RandomTexture>();
        m->random_diffuse_texture->setScale(20.0);
        m->specular_coeff = Eigen::Vector3f(0.0, 0.0, 0.0);
        m->alpha_phong = 10;
        m->specular = false;

        Eigen::Translation3f t(0.0, 0.0, 0.0);
        Eigen::Affine3f ta = Eigen::Affine3f(t);
        const float size = 0.5;
        auto box = std::make_shared<CSG_Box>(
            Eigen::Vector3f(-size, -size, -size),
            Eigen::Vector3f(size, size, size),
            ta);
        scene.addObject(box, m);
    }

    std::vector<ImageInfo> images_info;
    std::filesystem::path image_out_dir = output_dir / "images";
    std::filesystem::create_directories(image_out_dir);

    // generate points on the surface of unit sphere
    std::vector<int> ring_length = {10, 20, 32, 20, 10};
    std::vector<float> pitch_angle = {-1. / 3. * M_PI, -1. / 6. * M_PI, 0.0, 1. / 6. * M_PI, 1. / 3. * M_PI};
    for (int i = 0; i < ring_length.size(); i++) {
        for (int j = 0; j < ring_length.at(i); j++) {
            const float pitch = pitch_angle.at(i);
            const float theta = 2. * M_PI * j / ring_length.at(i);
            const double x = std::cos(theta) * std::cos(pitch) * cam_radius;
            const double y = std::sin(theta) * std::cos(pitch) * cam_radius;
            const double z = std::sin(pitch) * cam_radius;
            cam.setPosition(Eigen::Vector3f(x, y, z));
            cam.lookAt(Eigen::Vector3f(0.0, 0.0, 0.0));
            const std::string filename = "saved-" + std::to_string(i) + "-" + std::to_string(j) + ".png";
            std::cout << "Rendering " << filename << std::endl;
            cv::Mat image = render.renderImage(cam, scene);
            std::cout << "Saving " << filename << std::endl;
            cv::imwrite(image_out_dir / filename, image);

            auto &info = images_info.emplace_back();
            info.filename = filename;
            info.translation = cam.getTranslationW2C();
            Eigen::Matrix3f R = cam.getRotationW2C();
            info.quat = Eigen::Quaternionf(R);
        }
    }

    std::vector<Eigen::Vector3f> points3d;

    {
        // generate random points in a sphere
        int N = 1000;
        const float radius = cam_radius * 0.8;
        std::uniform_real_distribution<float> dist(-radius, radius);
        std::default_random_engine gen(123);
        while (N >= 0) {
            Eigen::Vector3f p(dist(gen), dist(gen), dist(gen));
            if (p.norm() < radius) {
                points3d.emplace_back(p);
                N--;
            }
        }
    }

    std::filesystem::path colmap_out_dir = output_dir / "sparse";
    std::filesystem::create_directories(colmap_out_dir);
    ConvertToColmapOutput(images_info, cam, width, height, focal, points3d, colmap_out_dir);
    std::filesystem::path colmap_out_dir2 = output_dir / "sparse" / "0";
    std::filesystem::create_directories(colmap_out_dir2);
    ConvertToColmapOutput(images_info, cam, width, height, focal, points3d, colmap_out_dir2);
}
