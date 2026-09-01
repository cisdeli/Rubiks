cmake_minimum_required(VERSION 3.18)

# Pining cuda
set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
set(CMAKE_CUDA_ARCHITECTURES 89)          # Ada

project(RMRubiks LANGUAGES C CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)

# Viewer
add_executable(viewer
    src/viewer.cpp
    external/src/glad.c
)
target_include_directories(viewer PRIVATE external/include ${GLFW3_INCLUDE_DIRS})
target_compile_definitions(viewer PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(viewer PRIVATE ${GLFW3_LIBRARIES} dl)

# Trainer
add_executable(trainer
    src/trainer.cu
)
target_include_directories(trainer PRIVATE external)

# Tests
add_executable(tests
    src/tests.cpp
)
target_include_directories(tests PRIVATE external)

# Cube library
add_library(cube STATIC src/cube.cpp)
target_include_directories(cube PUBLIC src)

target_link_libraries(viewer  PRIVATE cube ${GLFW3_LIBRARIES} dl)
target_link_libraries(tests   PRIVATE cube)
target_link_libraries(trainer PRIVATE cube)

