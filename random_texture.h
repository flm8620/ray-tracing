#pragma once

#include <random>
#include <Eigen/Dense>

class RandomNoise {
  private:
    const int seed;
    const int octaves = 5;
    const int N = 128;
    std::vector<std::vector<float>> noise_cubes;

  public:
    RandomNoise(const int seed_ = 42) : seed(seed_) {
        std::uniform_real_distribution<double> dis(0.0, 1.0);
        std::mt19937 gen(seed);
        noise_cubes.resize(octaves);
        for (int i = 0; i < octaves; i++) {
            noise_cubes[i].resize(N * N * N);
            for (int x = 0; x < N; x++) {
                for (int y = 0; y < N; y++) {
                    for (int z = 0; z < N; z++) {
                        noise_cubes[i][x + y * N + z * N * N] = dis(gen);
                    }
                }
            }
        }
    }

    double smoothTurbulenceNoise(double x, double y, double z, int octaves = 5) {
        double value = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0 / (1 << (octaves - 1));
        double denom = 0.0;
        int parent_seed = seed;
        for (int octave = 0; octave < octaves; octave++) {
            value += amplitude * smoothNoise(
                                     x * frequency, y * frequency, z * frequency,
                                     octave);
            denom += amplitude;
            amplitude *= 0.5;
            frequency *= 2.0;
        }
        return value / denom;
    }

  private:
    double generateNoise(int x, int y, int z, int octave) {
        return noise_cubes[octave][x + y * N + z * N * N];
    }

    double smoothNoise(double x, double y, double z, int octave) {
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
        value += (1 - fractX) * (1 - fractY) * (1 - fractZ) * generateNoise(x1, y1, z1, octave);
        value += (1 - fractX) * (1 - fractY) * fractZ * generateNoise(x1, y1, z2, octave);
        value += (1 - fractX) * fractY * (1 - fractZ) * generateNoise(x1, y2, z1, octave);
        value += (1 - fractX) * fractY * fractZ * generateNoise(x1, y2, z2, octave);

        value += fractX * (1 - fractY) * (1 - fractZ) * generateNoise(x2, y1, z1, octave);
        value += fractX * (1 - fractY) * fractZ * generateNoise(x2, y1, z2, octave);
        value += fractX * fractY * (1 - fractZ) * generateNoise(x2, y2, z1, octave);
        value += fractX * fractY * fractZ * generateNoise(x2, y2, z2, octave);
        return value;
    }
};
// RGB 3D noise texture
class RandomTexture {
  private:
    RandomNoise noise;
    double scale = 1.0;

  public:
    RandomTexture(int seed = 42) : noise(seed) {
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
