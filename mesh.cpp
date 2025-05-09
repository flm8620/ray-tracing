#include "mesh.h"

#include "ply_read.h"
#include "geometry_util.h"
#include "ray_slab.h"

using namespace std;
using namespace Eigen;

Mesh::Mesh() {
}

void Mesh::readPlyFile(const char *file) {
    ply_reader::t_ply_result ply_result;
    ply_reader::read_all_in_ply(file, ply_result);

    faceID face_startID = face_index.size();
    vertexID vertex_startID = vertices.size();
    face_index.resize(face_startID + ply_result.face_index.size());
    for (faceID i = 0; i < ply_result.face_index.size(); i++) {
        vertexID v0 = ply_result.face_index[i][0];
        vertexID v1 = ply_result.face_index[i][1];
        vertexID v2 = ply_result.face_index[i][2];
        face_index[i + face_startID][0] = v0 + vertex_startID;
        face_index[i + face_startID][1] = v1 + vertex_startID;
        face_index[i + face_startID][2] = v2 + vertex_startID;
    }
    face_normals.resize(face_startID + ply_result.face_index.size());
    vertices.resize(vertex_startID + ply_result.vertices.size());
    vertex_to_face.resize(vertex_startID + ply_result.vertices.size());
    vertex_normals.resize(vertex_startID + ply_result.vertices.size());

    size_t i = vertex_startID;
    for (auto &v : ply_result.vertices) {
        vertices[i][0] = v[0];
        vertices[i][1] = v[1];
        vertices[i][2] = v[2];
        i++;
    }

    for (faceID i = face_startID; i < face_index.size(); i++) {
        vertexID index0 = face_index[i][0];
        vertexID index1 = face_index[i][1];
        vertexID index2 = face_index[i][2];
        vertex_to_face[index0].push_back(i);
        vertex_to_face[index1].push_back(i);
        vertex_to_face[index2].push_back(i);
        Vector3f &v0 = vertices[index0];
        Vector3f &v1 = vertices[index1];
        Vector3f &v2 = vertices[index2];

        float rect_min[3], rect_max[3];
        rect_min[0] = min(v0[0], min(v1[0], v2[0]));
        rect_min[1] = min(v0[1], min(v1[1], v2[1]));
        rect_min[2] = min(v0[2], min(v1[2], v2[2]));
        rect_max[0] = max(v0[0], max(v1[0], v2[0]));
        rect_max[1] = max(v0[1], max(v1[1], v2[1]));
        rect_max[2] = max(v0[2], max(v1[2], v2[2]));
        tree.Insert(rect_min, rect_max, i);

        Vector3f n = (v1 - v0).cross(v2 - v0);
        n.normalize();
        face_normals[i] = n;
    }


    for (i = vertex_startID; i < vertex_to_face.size(); i++) {
        std::vector<faceID> &face_list = vertex_to_face[i];
        Vector3f n(0.0, 0.0, 0.0);
        for (faceID face : face_list)
            n += face_normals[face];
        n /= face_list.size();
        n.normalize();
        vertex_normals[i] = n;
    }
}

Eigen::AlignedBox3f Mesh::getBoundingBox() const {
    float min[3], max[3];
    tree.GetBoundingBox(min, max);
    Eigen::Vector3f vmin = {min[0], min[1], min[2]};
    Eigen::Vector3f vmax = {max[0], max[1], max[2]};
    return Eigen::AlignedBox3f(vmin, vmax);
}

bool Mesh::rayIntersect(const Vector3f &rayO, const Vector3f &rayD, IntersectReport &report) const {
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

    vector<faceID> suspects;
    MyTree::t_resultCallback suspect_callback =
        [](faceID value, void *suspects) -> bool {
        vector<faceID> *p = static_cast<vector<faceID> *>(suspects);
        p->push_back(value);
        return true;
    };

    int count = tree.Search_user_defined(static_cast<void *>(&ray), intersectFun, suspect_callback, &suspects);
    if (count > 0) {
        Eigen::Vector3f v1, v2, v3, O, D;
        O[0] = ray.O[0];
        O[1] = ray.O[1];
        O[2] = ray.O[2];
        D[0] = ray.D[0];
        D[1] = ray.D[1];
        D[2] = ray.D[2];
        D.normalize();
        float distance = numeric_limits<float>::max();
        faceID id;
        bool found = false;
        Vector3f intersect_point;
        Vector3f closest_intersect_point;
        float u, v, w;
        for (faceID i : suspects) {
            v1 = vertices[face_index[i][0]];
            v2 = vertices[face_index[i][1]];
            v3 = vertices[face_index[i][2]];
            float d, uu, vv, ww;
            if (ray_triangle_intersect(v1, v2, v3, rayO, rayD, &d, &intersect_point, &uu, &vv, &ww)) {
                if (d < distance) {
                    found = true;
                    id = i;
                    closest_intersect_point = intersect_point;
                    u = uu, v = vv, w = ww;
                }
                distance = std::min(distance, d);
            }
        }
        if (found) {
            vertexID v0 = face_index[id][0];
            vertexID v1 = face_index[id][1];
            vertexID v2 = face_index[id][2];
            report.normal = vertex_normals[v0] * w + vertex_normals[v1] * u + vertex_normals[v2] * v;
            report.normal.normalize();
            // report->normal = face_normals[id];
            report.intersect_point = closest_intersect_point;
            report.t = distance;
            return true;
        }
    }

    return false;
}

Mesh::Face Mesh::getFace(faceID id) const {
    Face f;
    f.v0 = vertices[face_index[id][0]];
    f.v1 = vertices[face_index[id][1]];
    f.v2 = vertices[face_index[id][2]];
    return f;
}

bool Mesh::rayIntersectWithIntervals(const Eigen::Vector3f &rayO,
                                     const Eigen::Vector3f &rayD,
                                     DisjointIntervals &intervals) const {
    throw; // not implemented
}