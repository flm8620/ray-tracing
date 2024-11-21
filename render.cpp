#include "render.h"

#include <iostream>
#include <algorithm>
#include <cmath>

#include <Eigen/Dense>

#include <glog/logging.h>

using namespace std;
using namespace Eigen;

Render::Render() {
}

float Render::considerFogInfluence(const IntervalIntersectReport &interval_report, float intensity) {
    struct Event {
        float t;
        int type; // 1 for entering, -1 for exiting
        Material *material;
        bool operator<(const Event &other) const {
            if (t != other.t)
                return t > other.t;   // Reverse order for integration from infinity to camera
            return type < other.type; // Exiting before entering at the same t
        }
    };
    std::vector<Event> events;
    // Collect events from all intervals and materials
    for (size_t i = 0; i < interval_report.objs_intervals.size(); ++i) {
        const DisjointIntervals &intervals = interval_report.objs_intervals[i];
        Material *material = interval_report.materials[i];
        for (const auto &interval : intervals) {
            events.push_back(Event{interval.left, 1, material});
            events.push_back(Event{interval.right, -1, material});
        }
    }
    // Sort events in reverse order (from infinity to camera)
    std::sort(events.begin(), events.end());

    // Initialize intensity at infinity
    float I = intensity;
    float last_t = 1e10;
    std::set<Material *> active_materials;
    for (const auto &e : events) {
        float t = e.t;
        if (t < 0.0f) {
            break;
        }
        if (t < last_t) {
            // Interval between last_t and t
            float delta_t = last_t - t;
            // Compute combined fog_sigma and fog_color
            float sigma = 0.0f;
            float sigma_color = 0.0f;
            for (auto mat : active_materials) {
                sigma += mat->fog_sigma;
                sigma_color += mat->fog_sigma * mat->fog_color;
            }
            float fog_color = 1.0f; // Default value
            if (sigma > 0.0f) {
                fog_color = sigma_color / sigma;
            }
            // Compute transmittance
            float transmittance = exp(-sigma * delta_t);
            // Update intensity
            I = I * transmittance + fog_color * (1.0f - transmittance);
        }
        // Update active materials
        if (e.type == -1) {
            CHECK(active_materials.insert(e.material).second);
        } else {
            CHECK_EQ(active_materials.erase(e.material), 1);
        }
        last_t = t;
    }
    // Process interval from t=0 to last_t (camera to last event)
    if (last_t > 0.0f) {
        float delta_t = last_t - 0.0f;
        float sigma = 0.0f;
        float sigma_color = 0.0f;
        for (auto mat : active_materials) {
            sigma += mat->fog_sigma;
            sigma_color += mat->fog_sigma * mat->fog_color;
        }
        float fog_color = 1.0f; // Default value
        if (sigma > 0.0f) {
            fog_color = sigma_color / sigma;
        }
        float transmittance = exp(-sigma * delta_t);
        I = I * transmittance + fog_color * (1.0f - transmittance);
    }
    return I;
}

float Render::getIntensity(const Scene &scene, Vector3f &rayO, Vector3f &rayD, int reflexLeft) {
    const float eps = 1e-6;
    IntersectReport report;
    Material *mt;
    IntervalIntersectReport interval_report;
    bool intersect = scene.ray_intersect_query(rayO, rayD, report, &mt, &interval_report);

    float intensity = 0.0;

    if (intersect) {
        if (mt->random_diffuse_texture) {
            float texture_value = mt->random_diffuse_texture->smoothTurbulenceNoise(report.intersect_point.x(),
                                                                                    report.intersect_point.y(),
                                                                                    report.intersect_point.z());
            intensity += lights.ambientIntensity * texture_value;
        } else {
            intensity += lights.ambientIntensity * mt->diffuse_coeff;
        }
        Vector3f n_to_ray = report.normal;
        if (report.normal.dot(rayD) > 0)
            n_to_ray = -n_to_ray;

        Vector3f offset_intersect = report.intersect_point + eps * n_to_ray;
        // sunshine
        Vector3f sun_rayO = offset_intersect;
        for (Sunshine s : lights.sunshines) {
            Vector3f sun_rayD = -s.direction;
            if (sun_rayD.dot(n_to_ray) < 0)
                continue;
            IntersectReport report_sun;
            Material *mt2;
            IntervalIntersectReport interval_report2;
            bool intersect_sun = scene.ray_intersect_query(sun_rayO, sun_rayD, report_sun, &mt2);
            if (!intersect_sun) {
                intensity += phongShading(n_to_ray, -rayD, sun_rayD, s.intensity, *mt);
            }
        }

        float reflex_intensity = 0;
        // reflex
        if (mt->mirror && reflexLeft > 0) {
            Vector3f r = -2.0 * rayD.dot(n_to_ray) * n_to_ray + rayD;
            Vector3f reflexD = r;
            Vector3f reflexO = offset_intersect;
            reflex_intensity = mt->specular_coeff * getIntensity(scene, reflexO, reflexD, reflexLeft - 1);
        }

        float T = 0.0, R = 1.0;
        // refraction
        if (mt->transparent && reflexLeft > 0) {
            float n1, n2;
            if (report.normal.dot(rayD) > 0) {
                n1 = mt->relative_refractive_index;
                n2 = 1.0;
            } else {
                n2 = mt->relative_refractive_index;
                n1 = 1.0;
            }
            Vector3f v = -rayD;
            Vector3f v_par = v - n_to_ray * (v.dot(n_to_ray));
            float sin_i = v_par.norm();
            float sin_r = n1 * sin_i / n2;

            if (sin_r < 1) {
                float cos_i = std::sqrt(1 - sin_i * sin_i);
                float cos_r = std::sqrt(1 - sin_r * sin_r);
                float n1i = n1 * cos_i;
                float n1r = n1 * cos_r;
                float n2i = n2 * cos_i;
                float n2r = n2 * cos_r;
                float Rs = (n1i - n2r) * (n1i - n2r) / ((n1i + n2r) * (n1i + n2r));
                float Rp = (n1r - n2i) * (n1r - n2i) / ((n1r + n2i) * (n1r + n2i));
                R = 0.5 * (Rs + Rp);
                T = 1 - R;
                Vector3f p = v.cross(n_to_ray).cross(n_to_ray);
                p.normalize();
                Vector3f t = -n_to_ray * cos_r + p * sin_r;
                Vector3f refractionD = t;
                Vector3f refractionO = report.intersect_point - eps * n_to_ray;
                intensity += T * mt->specular_coeff * getIntensity(scene, refractionO, refractionD, reflexLeft - 1);
            } else {
                R = 1.0;
            }
        }
        intensity += R * reflex_intensity;
    } else {
        intensity = scene.getBackground();
    }

    if (!interval_report.objs_intervals.empty()) {
        intensity = considerFogInfluence(interval_report, intensity);
    }

    return intensity;
}

float Render::phongShading(const Vector3f &n, const Vector3f &view, const Vector3f &light, float lightIntensity, const Material &mat) {
    float intensity = 0.0;
    intensity += std::max<float>(0.0, light.dot(n)) * lightIntensity * mat.diffuse_coeff;
    if (mat.specular) {
        Vector3f r;
        r = 2.0 * light.dot(n) * n - light;
        //    float lambda = 1 - r.dot(view);
        //    const int gamma = 4;
        //    float beta = mat.alpha_phong / gamma;
        //    float tmp = 1 - beta * lambda;
        //    float tmp2 = tmp * tmp;
        //    intensity += tmp2*tmp2*mat.specular_coeff*lightIntensity;
        intensity += pow(r.dot(view), mat.alpha_phong) * mat.specular_coeff * lightIntensity;
    }
    return intensity;
}

cv::Mat Render::renderImage(const Camera &cam, const Scene &scene) {
    const int H = cam.height, W = cam.width;
    const float focal = cam.focal;

    const float center_x = W / 2.0;
    const float center_y = H / 2.0;
    cv::Mat image(H, W, CV_8UC3);
    lights = scene.getAllLights();
    Vector3f o(cam.x(), cam.y(), cam.z());
    const int reflection = 10;
    #pragma omp parallel for schedule(dynamic, 16)
    for (int i = 0; i < H; i++) {
        cout << i << endl;
        for (int j = 0; j < W; j++) {
            Vector3f v;
            v[0] = j - center_x;
            v[1] = i - center_y;
            v[2] = focal;
            v.normalize();
            v = cam.rotationMatrix() * v;

            float intensity = getIntensity(scene, o, v, 10);
            intensity = std::max<float>(std::min<float>(intensity, 1.0f), 0.0f);
            image.at<cv::Vec3b>(i, j) = cv::Vec3b(255 * intensity, 255 * intensity, 255 * intensity);
        }
    }
    return image;
}
