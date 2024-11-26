cmake_minimum_required(VERSION 3.15)
project(ray_tracing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(GTest REQUIRED)
find_package(glog REQUIRED)

set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)


add_subdirectory(lib/rply)

add_library(ray_lib
  RTree.h 
  rtree_util.h
  rtree_util.cpp
  ray_slab.h 
  geometry_util.h 
  camera.h 
  camera.cpp
  render.h 
  render.cpp 
  scene.cpp 
  scene.h 
  intervals.cpp 
  intervals.h 
  intersectable.h 
  mesh.h 
  mesh.cpp 
  csg.h 
  csg.cpp
  random_texture.h
  random_texture.cpp
  colmap_convert.h
)

target_compile_options(ray_lib PRIVATE ${OpenMP_CXX_FLAGS})
target_link_libraries(ray_lib ${OpenCV_LIBS} Eigen3::Eigen ply_reader glog::glog OpenMP::OpenMP_CXX)
add_executable(ray main.cpp example_sphere_fog.cpp example_sphere_rgb.cpp example_fish_tank.cpp)
target_link_libraries(ray ray_lib)

add_executable(textureTest texture_test.cpp)
target_link_libraries(textureTest ray_lib)


enable_testing()

add_executable(runUnitTests test.cpp)
add_executable(testInterval test_interval.cpp intervals.h intervals.cpp)

target_link_libraries(runUnitTests PRIVATE ray_lib GTest::GTest GTest::Main ${OpenCV_LIBS})
target_link_libraries(testInterval PRIVATE ray_lib GTest::GTest GTest::Main Eigen3::Eigen)

add_test(NAME test COMMAND runUnitTests)
add_test(NAME test_interval COMMAND testInterval)
