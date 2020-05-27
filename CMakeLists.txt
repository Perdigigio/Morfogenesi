cmake_minimum_required(VERSION 3.0)

project(Morfogenesi)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(glad)
add_subdirectory(ext/glfw)
add_subdirectory(core)
