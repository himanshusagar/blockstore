cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

# CMakeLists.txt
set (VCPKG_HOME ~/dev/install/vcpkg/) #IMPORTANT: change this to where your vcpkg exists.
set (CMAKE_TOOLCHAIN_FILE "${VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake")
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_BUILD_TYPE Release)
set (CMAKE_MAKE_PROGRAM make CACHE FILEPATH "" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(blockstore)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(benchmark CONFIG REQUIRED)


add_subdirectory(external)
add_subdirectory(src)

