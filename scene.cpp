#include "scene.h"

#include <algorithm>
#include <vector>
#include <cassert>

#include "ray_slab.h"
#include "mesh.h"

using namespace std;
using namespace Eigen;

Scene::Scene() {
}

void Scene::addMeshFromPlyFile(const char *file, const std::shared_ptr<Material> &material) {
    std::shared_ptr<Mesh> mesh(new Mesh());
    mesh->readPlyFile(file);
    addObject(mesh, material);
}

void Scene::addObject(std::shared_ptr<Intersectable> obj, const std::shared_ptr<Material> &material) {
    float box_min[3], box_max[3];
    Eigen::AlignedBox3f box;
    box = obj->getBoundingBox();
    if (box.isEmpty())
        return;
    Eigen::Vector3f vmin = box.min(), vmax = box.max();
    box_min[0] = vmin[0];
    box_min[1] = vmin[1];
    box_min[2] = vmin[2];
    box_max[0] = vmax[0];
    box_max[1] = vmax[1];
    box_max[2] = vmax[2];

    assert(objects.size() == objects_material.size());
    objectID id = objects.size();
    tree.Insert(box_min, box_max, id);
    objects.push_back(obj);
    materialID m_id = materials.size();
    objects_material.push_back(m_id);
    materials.push_back(material);
}


void Scene::addSunshine(Sunshine s) {
    s.direction.normalize();
    lights.sunshines.push_back(s);
}

bool Scene::ray_intersect_query(const Vector3f &rayO, const Vector3f &rayD,
                                IntersectReport &report, Material **material,
                                IntervalIntersectReport *interval_report) const {
    struct RaySimple {
        float O[3], D[3];
    } ray;
    ray.O[0] = rayO[0];
    ray.O[1] = rayO[1];
    ray.O[2] = rayO[2];
    ray.D[0] = rayD[0];
    ray.D[1] = rayD[1];
    ray.D[2] = rayD[2];

    MyTree::intersect_test intersectFun = [](void *userData, MyTree::Rect *rect) -> bool {
        RaySimple *p = static_cast<RaySimple *>(userData);
        return slab_test_3d<float>(p->O, p->D, rect->m_min, rect->m_max);
    };

    std::vector<objectID> suspects;
    MyTree::t_resultCallback suspect_callback =
        [](objectID value, void *suspects) -> bool {
        std::vector<objectID> *p = static_cast<vector<objectID> *>(suspects);
        p->push_back(value);
        return true;
    };
    int count = tree.Search_user_defined(static_cast<void *>(&ray), intersectFun, suspect_callback, &suspects);
    if (count <= 0)
        return false;

    float distance = numeric_limits<float>::max();
    bool found = false;
    IntersectReport report_tmp;
    objectID obj_id;
    for (objectID i : suspects) {
        if (materials[objects_material[i]]->is_fog) {
            if (interval_report) {
                DisjointIntervals intervals;
                if (objects[i]->rayIntersectWithIntervals(rayO, rayD, intervals)) {
                    interval_report->objs_intervals.push_back(intervals);
                    interval_report->materials.push_back(materials[objects_material[i]].get());
                }
            }
        } else {
            if (!objects[i]->rayIntersect(rayO, rayD, report_tmp))
                continue;
            if (report_tmp.t < distance) {
                found = true;
                obj_id = i;
                report = report_tmp;
            }
            distance = std::min(distance, report_tmp.t);
        }
    }
    if (found) {
        *material = materials[objects_material[obj_id]].get();

        if (interval_report) {
            DisjointIntervals::Interval ray_interval;
            ray_interval.left = 0;
            ray_interval.right = distance;

            DisjointIntervals disjoint_ray_interval({ray_interval});
            for (auto &interval : interval_report->objs_intervals) {
                interval.intersectionWith(disjoint_ray_interval);
            }
        }
        return true;
    } else {
        return false;
    }
}

void Scene::setAmbientColor(const Eigen::Vector3f &color) {
    lights.ambientColor = color;
}

Eigen::Vector3f Scene::getAmbientColor() const {
    return lights.ambientColor;
}

Lights Scene::getAllLights() const {
    return lights;
}
