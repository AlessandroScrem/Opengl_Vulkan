cmake_minimum_required(VERSION 3.11)
project(AppRunner VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

include(CTest)
enable_testing()

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Vulkan REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(lib)
add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
