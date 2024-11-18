
#include <opencv2/opencv.hpp>

#include "random_texture.h"


int main() {
    int H = 100;
    int W = 100;
    RandomNoise noise;
    cv::Mat im(H, W, CV_8U);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            // im.at<uint8_t>(i, j) = std::clamp((int)std::round(255 * noise.smoothNoise(i*0.1, j, 10)), 0, 255);
            im.at<uint8_t>(i, j) = std::clamp((int)std::round(255 * noise.smoothTurbulenceNoise(0, i-100, j)), 0, 255);
        }
    }
    cv::imwrite("noise.png", im);
    return 0;
}