
add_compile_options(-std=c++11)



add_library(rply rply.c)
target_include_directories(rply PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
