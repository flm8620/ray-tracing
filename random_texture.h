#pragma once

#include <random>
#include <Eigen/Dense>

class RandomNoise {
  private:
    int seed;

  public:
    RandomNoise(int seed_ = 42) {
        seed = seed_;
    }

    void setSeed(int seed_) {
        seed = seed_;
    }

    double smoothTurbulenceNoise(double x, double y, double z, int octaves = 5) {
        double value = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0 / (1 << (octaves - 1));
        double denom = 0.0;
        int parent_seed = seed;
        for (int i = 0; i < octaves; i++) {
            int level_seed = parent_seed * 991 % 857;
            value += amplitude * smoothNoise(x * frequency, y * frequency, z * frequency, level_seed);
            denom += amplitude;
            amplitude *= 0.5;
            frequency *= 2.0;
        }
        return value / denom;
    }

  private:
    double generateNoise(int x, int y, int z, int seed) {
        const int N = 128;
        std::uniform_real_distribution<double> dis(0.0, 1.0);
        std::mt19937 gen;
        gen.seed(seed * (x + y * N + z * N * N));
        return dis(gen);
    }

    double smoothNoise(double x, double y, double z, int seed) {
        const int N = 128;
        int xn = std::floor(x);
        int yn = std::floor(y);
        int zn = std::floor(z);

        double fractX = x - xn;
        double fractY = y - yn;
        double fractZ = z - zn;

        int mx = xn / N;
        int my = yn / N;
        int mz = zn / N;

        int x1 = (xn - mx * N + N) % N;
        int y1 = (yn - my * N + N) % N;
        int z1 = (zn - mz * N + N) % N;

        int x2 = (x1 + 1) % N;
        int y2 = (y1 + 1) % N;
        int z2 = (z1 + 1) % N;

        double value = 0.0;
        value += (1 - fractX) * (1 - fractY) * (1 - fractZ) * generateNoise(x1, y1, z1, seed);
        value += (1 - fractX) * (1 - fractY) * fractZ * generateNoise(x1, y1, z2, seed);
        value += (1 - fractX) * fractY * (1 - fractZ) * generateNoise(x1, y2, z1, seed);
        value += (1 - fractX) * fractY * fractZ * generateNoise(x1, y2, z2, seed);

        value += fractX * (1 - fractY) * (1 - fractZ) * generateNoise(x2, y1, z1, seed);
        value += fractX * (1 - fractY) * fractZ * generateNoise(x2, y1, z2, seed);
        value += fractX * fractY * (1 - fractZ) * generateNoise(x2, y2, z1, seed);
        value += fractX * fractY * fractZ * generateNoise(x2, y2, z2, seed);
        return value;
    }
};
// RGB 3D noise texture
class RandomTexture {
  private:
    RandomNoise noise;
    double scale = 1.0;

  public:
    RandomTexture(int seed = 42) {
        noise.setSeed(seed);
    }

    void setSeed(int seed) {
        noise.setSeed(seed);
    }

    void setScale(double scale_) {
        scale = scale_;
    }

    Eigen::Vector3f getColor(double x, double y, double z) {
        x = x * scale;
        y = y * scale;
        z = z * scale;
        double r = noise.smoothTurbulenceNoise(x, y, z, 5);
        double g = noise.smoothTurbulenceNoise(x + 100, y + 100, z + 100, 5);
        double b = noise.smoothTurbulenceNoise(x + 200, y + 200, z + 200, 5);
        return Eigen::Vector3f(r, g, b);
    }
};
