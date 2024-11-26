#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <random>

#include <Eigen/Dense>

#include "camera.h"

struct ImageInfo {
    std::string filename;
    Eigen::Vector3f translation;
    Eigen::Quaternionf quat;
};

inline void ConvertToColmapOutput(const std::vector<ImageInfo> &images_info,
                                  const Camera &cam,
                                  const int width,
                                  const int height,
                                  const float focal,
                                  const std::vector<Eigen::Vector3f> &points3d,
                                  const std::filesystem::path &output_dir) {
    std::filesystem::create_directories(output_dir);
    // write to COLMAP output format as 3 txt files
    {
        // write camera txt
        std::ofstream cam_file(output_dir / "cameras.txt");
        cam_file << "0 PINHOLE "
                 << width << " "
                 << height << " "
                 << focal << " "
                 << focal << " "
                 << cam.cx() << " "
                 << cam.cy() << " " << std::endl;
    }
    {
        // write images txt
        std::ofstream images_file(output_dir / "images.txt");
        for (int i = 0; i < images_info.size(); i++) {
            const auto &info = images_info[i];
            images_file << i << " "
                        << info.quat.w() << " "
                        << info.quat.x() << " "
                        << info.quat.y() << " "
                        << info.quat.z() << " "
                        << info.translation.x() << " "
                        << info.translation.y() << " "
                        << info.translation.z() << " "
                        << "0" << " "
                        << info.filename << std::endl;
            images_file << "100 100 0" << std::endl;
        }
    }
    {
        // write points3D txt
        std::ofstream points3D_file(output_dir / "points3D.txt");
        for (int i = 0; i < points3d.size(); i++) {
            points3D_file << i << " "
                          << points3d[i].x() << " "
                          << points3d[i].y() << " "
                          << points3d[i].z() << " "
                          << "128 128 128 0 0 0" << std::endl;
        }

        points3D_file << std::endl;
    }
}