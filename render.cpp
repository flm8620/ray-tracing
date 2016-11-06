#include "render.h"
#include <iostream>
#include <Eigen/Dense>
#include <algorithm>
#include <cmath>
using namespace std;
using namespace Eigen;
Render::Render()
{

}

float Render::getIntensity(const Scene& scene, Ray& ray, int reflexLeft){
    const float eps = 1e-6;
    IntersectReport report;
    bool intersect = scene.ray_intersect_query(ray,&report);

    float intensity=0.0;
    if(intersect){
        const Material& mt = scene.getFaceMaterial(report.faceid);
        intensity+=lights.ambientIntensity * mt.diffuse_coeff;
        Vector3f n_to_ray = report.normal;
        if(report.normal.dot(ray.D) > 0) n_to_ray = -n_to_ray;

        //sunshine
        Ray ray_sun;
        ray_sun.O = report.intersect_point + eps * n_to_ray;
        for(Sunshine s : lights.sunshines){
            ray_sun.D = - s.direction;
            if(ray_sun.D.dot(n_to_ray) < 0) continue;
            IntersectReport report_sun;
            bool intersect_sun = scene.ray_intersect_query(ray_sun,&report_sun);
            if(!intersect_sun){
                intensity += phongShading(n_to_ray,-ray.D,ray_sun.D,s.intensity,mt);
            }
        }
    }

    return intensity;
}

float Render::phongShading(const Vector3f &n, const Vector3f &view, const Vector3f &light, float lightIntensity, const Material &mat)
{
    float intensity=0.0;
    intensity += std::max<float>(0.0,light.dot(n)) * lightIntensity * mat.diffuse_coeff;
    Vector3f r;
    r= 2.0 * light.dot(n) * n - light;
//    float lambda = 1 - r.dot(view);
//    const int gamma = 4;
//    float beta = mat.alpha_phong / gamma;
//    float tmp = 1 - beta * lambda;
//    float tmp2 = tmp * tmp;
//    intensity += tmp2*tmp2*mat.specular_coeff*lightIntensity;
    intensity += pow(r.dot(view),mat.alpha_phong) * mat.specular_coeff*lightIntensity;
    return intensity;
}

QImage Render::renderImage(const Camera& cam, const Scene& scene){
    float H=cam.height, W=cam.width,focal=cam.focal;

    float center_x=W/2, center_y=H/2;
    QImage image(W, H, QImage::Format_RGB32);
    lights = scene.getAllLights();
    Vector3f o(cam.x(),cam.y(),cam.z());
    for(int i=0;i<H;i++){
        for(int j=0;j<W;j++){
            Vector3f v;
            v[0]=j-center_x;
            v[1]=i-center_y;
            v[2]=focal;
            v.normalize();
            v = cam.rotationMatrix() * v;

            Ray ray; ray.O=o; ray.D=v;
            float intensity = getIntensity(scene, ray, 1);
            intensity = std::max<float>(std::min<float>(intensity,1.0f),0.0f);
            image.setPixel(j,i,qRgb(255*intensity,255*intensity,255*intensity));
        }
    }
    return image;

}
