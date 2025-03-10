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

void example_sphere_fog() {
    const std::filesystem::path output_dir = "sphere_fog_output";
    std::filesystem::create_directories(output_dir);

    Scene scene;
    scene.setBackground(Eigen::Vector3f::Zero());

    int width = 800;
    int height = 600;
    float focal = 600.;
    Camera cam(width, height, focal);

    Render render;

    {
        auto m = std::make_shared<Material>();
        m->is_fog = true;
        m->fog_sigma = 10.0;
        m->fog_color = Eigen::Vector3f(1.0, 1.0, 1.0);
        auto sphere = std::make_shared<CSG_Sphere>(Eigen::Vector3f(0.0, 0.0, 0.0), 0.1f);
        scene.addObject(sphere, m);
    }

    std::vector<ImageInfo> images_info;
    std::filesystem::path image_out_dir = output_dir / "images";
    std::filesystem::create_directories(image_out_dir);

    // generate points on the surface of unit sphere
    std::vector<int> ring_length = {5, 10, 16, 10, 5};
    std::vector<float> pitch_angle = {-1. / 3. * M_PI, -1. / 6. * M_PI, 0.0, 1. / 6. * M_PI, 1. / 3. * M_PI};
    for (int i = 0; i < ring_length.size(); i++) {
        for (int j = 0; j < ring_length.at(i); j++) {
            const float pitch = pitch_angle.at(i);
            const float theta = 2. * M_PI * j / ring_length.at(i);
            const double radius = 0.3;
            const double x = std::cos(theta) * std::cos(pitch) * radius;
            const double y = std::sin(theta) * std::cos(pitch) * radius;
            const double z = std::sin(pitch) * radius;
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
        // generate random points in a box
        const int N = 1000;
        const float halfsize = 0.12;
        std::uniform_real_distribution<float> dist(-halfsize, halfsize);
        std::default_random_engine gen(123);
        for (int i = 0; i < N; i++) {
            points3d.emplace_back(dist(gen), dist(gen), dist(gen));
        }
    }

    std::filesystem::path colmap_out_dir = output_dir / "sparse";
    std::filesystem::create_directories(colmap_out_dir);
    ConvertToColmapOutput(images_info, cam, width, height, focal, points3d, colmap_out_dir);
    std::filesystem::path colmap_out_dir2 = output_dir / "sparse" / "0";
    std::filesystem::create_directories(colmap_out_dir2);
    ConvertToColmapOutput(images_info, cam, width, height, focal, points3d, colmap_out_dir2);
}
