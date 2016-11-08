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

        Vector3f offset_intersect = report.intersect_point + eps * n_to_ray;
        //sunshine
        Ray ray_sun;
        ray_sun.O = offset_intersect;
        for(Sunshine s : lights.sunshines){
            ray_sun.D = - s.direction;
            if(ray_sun.D.dot(n_to_ray) < 0) continue;
            IntersectReport report_sun;
            bool intersect_sun = scene.ray_intersect_query(ray_sun,&report_sun);
            if(!intersect_sun){
                intensity += phongShading(n_to_ray,-ray.D,ray_sun.D,s.intensity,mt);
            }
        }

        float reflex_intensity=0;
        //reflex
        if(mt.mirror && reflexLeft > 0){
            Vector3f r = -2.0 * ray.D.dot(n_to_ray) * n_to_ray + ray.D;
            Ray reflex;
            reflex.D = r; reflex.O = offset_intersect;
            reflex_intensity = mt.specular_coeff * getIntensity(scene, reflex, reflexLeft-1);
        }

        float T=0.0, R=1.0;
        //refraction
        if(mt.transparent && reflexLeft > 0){
            float n1,n2;
            mt.relative_refractive_index;
            if(report.normal.dot(ray.D) > 0) {
                n1 = mt.relative_refractive_index;
                n2 = 1.0;
            }else{
                n2 = mt.relative_refractive_index;
                n1 = 1.0;
            }
            Vector3f v = - ray.D;
            Vector3f v_par = v - n_to_ray * (v.dot(n_to_ray));
            float sin_i = v_par.norm();
            float sin_r = n1 * sin_i / n2;

            if(sin_r < 1){
                float cos_i = std::sqrt(1-sin_i*sin_i);
                float cos_r = std::sqrt(1-sin_r*sin_r);
                float n1i = n1*cos_i;
                float n1r = n1*cos_r;
                float n2i = n2*cos_i;
                float n2r = n2*cos_r;
                float Rs = (n1i-n2r)*(n1i-n2r)/((n1i+n2r)*(n1i+n2r));
                float Rp = (n1r-n2i)*(n1r-n2i)/((n1r+n2i)*(n1r+n2i));
                R = 0.5*(Rs+Rp);
                T = 1-R;
                Vector3f p = v.cross(n_to_ray).cross(n_to_ray);
                p.normalize();
                Vector3f t = -n_to_ray*cos_r + p * sin_r;
                Ray refraction;
                refraction.D = t; refraction.O = report.intersect_point - eps * n_to_ray;
                intensity += T * mt.specular_coeff * getIntensity(scene, refraction, reflexLeft-1);
            }else{
                R=1.0;
            }


        }
        intensity += R * reflex_intensity;
    }


    return intensity;
}

float Render::phongShading(const Vector3f &n, const Vector3f &view, const Vector3f &light, float lightIntensity, const Material &mat)
{
    float intensity=0.0;
    intensity += std::max<float>(0.0,light.dot(n)) * lightIntensity * mat.diffuse_coeff;
    if(mat.specular){
        Vector3f r;
        r= 2.0 * light.dot(n) * n - light;
        //    float lambda = 1 - r.dot(view);
        //    const int gamma = 4;
        //    float beta = mat.alpha_phong / gamma;
        //    float tmp = 1 - beta * lambda;
        //    float tmp2 = tmp * tmp;
        //    intensity += tmp2*tmp2*mat.specular_coeff*lightIntensity;
        intensity += pow(r.dot(view),mat.alpha_phong) * mat.specular_coeff*lightIntensity;
    }
    return intensity;
}

QImage Render::renderImage(const Camera& cam, const Scene& scene){
    float H=cam.height, W=cam.width,focal=cam.focal;

    float center_x=W/2, center_y=H/2;
    QImage image(W, H, QImage::Format_RGB32);
    lights = scene.getAllLights();
    Vector3f o(cam.x(),cam.y(),cam.z());
    for(int i=0;i<H;i++){
        cout<<i<<endl;
        for(int j=0;j<W;j++){
            Vector3f v;
            v[0]=j-center_x;
            v[1]=i-center_y;
            v[2]=focal;
            v.normalize();
            v = cam.rotationMatrix() * v;

            Ray ray; ray.O=o; ray.D=v;
            float intensity = getIntensity(scene, ray, 5);
            intensity = std::max<float>(std::min<float>(intensity,1.0f),0.0f);
            image.setPixel(j,i,qRgb(255*intensity,255*intensity,255*intensity));
        }
    }
    return image;

}
