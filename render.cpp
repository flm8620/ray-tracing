#include "render.h"
#include <iostream>
#include <Eigen/Dense>
#include <algorithm>
using namespace std;
using namespace Eigen;
Render::Render()
{

}
static float getIntensity(const Scene& scene, Ray& ray, std::vector<Sunshine>& sunshines, int reflexLeft){
    const float eps = 1e-4;
    IntersectReport report;
    bool intersect = scene.ray_intersect_query(ray,&report);

    float intensity=0.0;
    if(intersect){
        intensity+=0.2;
        Ray ray_sun;
        ray_sun.O = report.intersect_point + eps * report.normal;
        for(Sunshine s : sunshines){
            ray_sun.D = - s.direction;
            IntersectReport report_sun;
            bool intersect_sun = scene.ray_intersect_query(ray_sun,&report_sun);
            if(!intersect_sun){
                intensity += std::max<float>(0.0,ray_sun.D.dot(report.normal)) * s.intensity;
            }
        }
    }

    return intensity;
}

QImage Render::renderImage(const Camera& cam, const Scene& scene){
    float H=cam.height, W=cam.width,focal=cam.focal;

    float center_x=W/2, center_y=H/2;
    QImage image(W, H, QImage::Format_RGB32);
    vector<Sunshine> sunshines = scene.getSunshines();
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
            float intensity = getIntensity(scene, ray, sunshines, 1);
            intensity = std::max<float>(std::min<float>(intensity,1.0f),0.0f);
            image.setPixel(j,i,qRgb(255*intensity,255*intensity,255*intensity));
        }
    }
    return image;

}
