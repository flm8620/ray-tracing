#pragma once

#include <opencv2/opencv.hpp>

#include "camera.h"
#include "scene.h"

class Render {
    Lights lights;

    float getIntensity(const Scene &scene, Eigen::Vector3f &rayO, Eigen::Vector3f &rayD, int reflexLeft);
    float phongShading(const Eigen::Vector3f &n,
                       const Eigen::Vector3f &view,
                       const Eigen::Vector3f &light,
                       float lightIntensity,
                       const Material &mt);

  public:
    Render();
    cv::Mat renderImage(const Camera &cam, const Scene &scene);
};
