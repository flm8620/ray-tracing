#ifndef PLY_READ_H
#define PLY_READ_H
#include <vector>
#include <array>

namespace ply_reader{

struct t_ply_result{
    std::vector<std::array<double,3>> vertices;
    std::vector<std::array<unsigned int,3>> face_index;
};

bool read_all_in_ply(const char* filename, t_ply_result& ply_result);
}

#endif // PLY_READ_H
