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

using namespace std;
using namespace Eigen;

int main() {
    Scene scene;
    scene.setBackground(Eigen::Vector3f::Zero());

    // Sunshine sun1;
    // sun1.direction = Vector3f(-0.3, 0.1, -1.0);
    // sun1.color = Eigen::Vector3f(0.8, 0.8, 0.8);
    // scene.addSunshine(sun1);

    // Sunshine sun2;
    // sun2.direction = Vector3f(-1.0, -1.0, -1.0);
    // sun2.color = Eigen::Vector3f(0.4, 0.4, 0.4);
    // scene.addSunshine(sun2);

    scene.setAmbientColor(Eigen::Vector3f(0.4, 0.4, 0.4));

    int width = 800;
    int height = 600;
    float focal = 600.;
    Camera cam(width, focal, focal);

    Render render;

    {
        auto m_fog = std::make_shared<Material>();
        m_fog->is_fog = true;
        m_fog->fog_sigma = 10.0;
        m_fog->fog_color = Eigen::Vector3f(1.0, 1.0, 1.0);
        auto sphere = std::make_shared<CSG_Sphere>(Vector3f(0.0, 0.0, 0.0), 0.1f);
        scene.addObject(sphere, m_fog);
    }

    std::vector<ImageInfo> images_info;

    const std::filesystem::path output_dir = "sphere_fog_output";
    std::filesystem::create_directories(output_dir);

    // generate points on the surface of unit sphere
    std::vector<int> ring_length = {3, 5, 8, 5, 3};
    std::vector<float> pitch_angle = {-1. / 3. * M_PI, -1. / 6. * M_PI, 0.0, 1. / 6. * M_PI, 1. / 3. * M_PI};
    for (int i = 0; i < ring_length.size(); i++) {
        for (int j = 0; j < ring_length.at(i); j++) {
            const float pitch = pitch_angle.at(i);
            const float theta = 2. * M_PI * j / ring_length.at(i);
            const double radius = 0.3;
            const double x = std::cos(theta) * std::cos(pitch) * radius;
            const double y = std::sin(theta) * std::cos(pitch) * radius;
            const double z = std::sin(pitch) * radius;
            cam.setPosition(Vector3f(x, y, z));
            cam.lookAt(Vector3f(0.0, 0.0, 0.0));
            cv::Mat image = render.renderImage(cam, scene);
            const std::string image_name = "saved-" + std::to_string(i) + "-" + std::to_string(j) + ".png";
            cv::imwrite(output_dir / image_name, image);

            auto &info = images_info.emplace_back();
            info.filename = image_name;
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

    ConvertToColmapOutput(images_info, cam, width, height, focal, points3d, output_dir / "colmap");


    return 0;
}
