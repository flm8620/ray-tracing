project(ray_tracing)
cmake_minimum_required(VERSION 2.8.12)
add_compile_options(-std=c++11)

set(CMAKE_DEBUG_TARGET_PROPERTIES
  INCLUDE_DIRECTORIES
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Widgets)


add_subdirectory(lib/rply)

add_executable(${PROJECT_NAME} main.cpp RTree.h ray_slab.h geometry_util.h geometry_util.cpp camera.h camera.cpp
    render.h render.cpp scene.cpp scene.h)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} ply_reader)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/eigen)

add_subdirectory(lib/gtest-1.6.0)


enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable(runUnitTests test.cpp)
target_link_libraries(runUnitTests gtest gtest_main)
target_link_libraries(runUnitTests Qt5::Widgets)
add_test(NAME that-test-I-made COMMAND runUnitTests)
