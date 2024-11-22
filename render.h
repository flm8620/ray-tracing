#pragma once

#include <opencv2/opencv.hpp>

#include "camera.h"
#include "scene.h"

class Render {
    Lights lights;
    Eigen::Vector3f considerFogInfluence(const IntervalIntersectReport &interval_report,
                                         const Eigen::Vector3f &color);
    Eigen::Vector3f getColor(const Scene &scene, Eigen::Vector3f &rayO, Eigen::Vector3f &rayD, int reflexLeft);
    Eigen::Vector3f phongShading(const Eigen::Vector3f &n,
                                 const Eigen::Vector3f &view,
                                 const Eigen::Vector3f &light,
                                 const Eigen::Vector3f &color,
                                 const Material &mt);

  public:
    Render();
    cv::Mat renderImage(const Camera &cam, const Scene &scene);
};
