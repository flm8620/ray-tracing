#include <iostream>
#include <vector>
#include <array>
#include "rply.h"
#include "ply_read.h"

namespace ply_reader{
static int vertex_cb(p_ply_argument argument) {
    long index;
    void* p;
    ply_get_argument_user_data(argument, &p, &index);
    t_ply_result* p_result;
    p_result = static_cast<t_ply_result*>(p);
    long instance_index;
    ply_get_argument_element(argument,NULL,&instance_index);
    double value = ply_get_argument_value(argument);
    p_result->vertices[instance_index][index]=value;
    return 1;
}

static int face_cb(p_ply_argument argument) {

    void* p;
    ply_get_argument_user_data(argument, &p, NULL);
    t_ply_result* p_result;
    p_result = static_cast<t_ply_result*>(p);

    long length, value_index;
    long instance_index;
    ply_get_argument_element(argument,NULL,&instance_index);
    ply_get_argument_property(argument, NULL, &length, &value_index);
    if(value_index<0) return 1;

    unsigned int value = ply_get_argument_value(argument);
    p_result->face_index[instance_index][value_index]=value;

    return 1;
}

bool read_all_in_ply(const char* filename, t_ply_result& ply_result){
    long nvertices, ntriangles;
    p_ply ply = ply_open(filename, NULL, 0, NULL);
    if (!ply) return false;
    if (!ply_read_header(ply)) return false;
    nvertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb, &ply_result, 0);
    ply_set_read_cb(ply, "vertex", "y", vertex_cb, &ply_result, 1);
    ply_set_read_cb(ply, "vertex", "z", vertex_cb, &ply_result, 2);
    ply_result.vertices.resize(nvertices);
    ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, &ply_result, 0);
    ply_result.face_index.resize(ntriangles);
    std::cout<<"nv nt "<<nvertices<<" "<<ntriangles<<std::endl;
    if (!ply_read(ply)) return false;
    ply_close(ply);
    return true;
}
}
