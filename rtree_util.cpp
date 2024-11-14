#include "rtree_util.h"

#include <iostream>
#include <fstream>

using namespace std;

void exportRTreeToPly(const MyTree& tree, const char *file) {
    ofstream plyfile;
    plyfile.open(file);
    plyfile << "ply\n"
               "format ascii 1.0\n"
               "comment author: Leman's Ray Tracing\n"
               "comment object: rtree"
            << endl;
    struct Box {
        float boundsMin[3];
        float boundsMax[3];
    };
    struct Vertex {
        float x, y, z;
        int r, g, b;
    };
    struct Rect {
        int v1, v2, v3, v4;
    };

    vector<pair<MyTree::Rect, int>> boxes;
    int total_depth;
    tree.GetTreeStructure(boxes, total_depth);
    vector<Vertex> vertices;
    vector<Rect> rects;
    // Iterator test

    for (auto &box : boxes) {
        if (box.second > 2)
            continue;
        float *a = box.first.m_min;
        float *b = box.first.m_max;
        int c = 255 - int(255. * box.second / total_depth);
        int k = vertices.size();
        vertices.push_back({a[0], a[1], a[2], c, c, c}); // 0
        vertices.push_back({b[0], a[1], a[2], c, c, c}); // 1
        vertices.push_back({a[0], b[1], a[2], c, c, c}); // 2
        vertices.push_back({b[0], b[1], a[2], c, c, c}); // 3
        vertices.push_back({a[0], a[1], b[2], c, c, c}); // 4
        vertices.push_back({b[0], a[1], b[2], c, c, c}); // 5
        vertices.push_back({a[0], b[1], b[2], c, c, c}); // 6
        vertices.push_back({b[0], b[1], b[2], c, c, c}); // 7

        rects.push_back({k + 0, k + 1, k + 3, k + 2});
        rects.push_back({k + 0, k + 2, k + 6, k + 4});
        rects.push_back({k + 2, k + 3, k + 7, k + 6});
        rects.push_back({k + 1, k + 5, k + 7, k + 3});
        rects.push_back({k + 4, k + 6, k + 7, k + 5});
        rects.push_back({k + 0, k + 4, k + 5, k + 1});
    }
    plyfile << "element vertex " << vertices.size() << endl
            << "property float x\n"
               "property float y\n"
               "property float z\n"
               "property uchar red\n"
               "property uchar green\n"
               "property uchar blue\n"
               "element face "
            << rects.size() << endl
            << "property list uchar int vertex_index\n"
               "end_header"
            << endl;
    for (auto &v : vertices) {
        plyfile << v.x << " " << v.y << " " << v.z << " "
                << v.r << " " << v.g << " " << v.b << endl;
    }
    for (auto &r : rects) {
        plyfile << "4 " << r.v1 << " " << r.v2 << " " << r.v3 << " " << r.v4 << endl;
    }
    plyfile.close();
}