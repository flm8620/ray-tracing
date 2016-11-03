
add_compile_options(-std=c++11)


add_subdirectory(rply)

add_library(ply_reader ply_read.cpp ply_read.h)
target_include_directories(ply_reader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ply_reader PRIVATE rply)
