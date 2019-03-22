cmake_minimum_required(VERSION 2.8.12)
project(minimalist_segregation)

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig)
pkg_check_modules(ARGOS REQUIRED argos3_simulator)
set(ARGOS_PREFIX ${ARGOS_PREFIX} CACHE INTERNAL "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)

include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS})
link_directories(${ARGOS_LIBRARY_DIRS})

add_subdirectory(controllers)
add_subdirectory(loop_functions)
add_subdirectory(bin)
