#include "scene.h"
#include <algorithm>
#include "ray_slab.h"
#include "geometry_util.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
using namespace Eigen;
Scene::Scene()
{

}

void Scene::addMeshFromPlyFile(const char *file)
{

}



void Scene::addSunshine(Sunshine s)
{
    s.direction.normalize();
    lights.sunshines.push_back(s);
}

void Scene::setAmbientIntensity(float I)
{
    lights.ambientIntensity=I;
}

float Scene::getAmbientIntensity()const
{
    return lights.ambientIntensity;
}

Lights Scene::getAllLights() const
{
    return lights;
}


