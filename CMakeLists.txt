cmake_minimum_required(VERSION 3.6)

set(PROJECT_NAME "Fast Infinite Tilemap")
set(EXECUTABLE_NAME "Tilemap")
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS false)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -march=native")

set(CMAKE_SKIP_BUILD_RPATH false)
set(CMAKE_BUILD_WITH_INSTALL_RPATH false)
set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME})

set(BUILD_SHARED_LIBS false)

add_subdirectory(${SOURCE_DIR})
