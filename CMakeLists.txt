cmake_minimum_required(VERSION 2.8.12)
project(RobotAggregation)

option(CLUSTER "set to ON if you're building on the cluster" OFF)
set(CMAKE_CXX_STANDARD 14)

# Find the ARGoS package, make sure to save the ARGoS prefix
find_package(PkgConfig)
pkg_check_modules(ARGOS REQUIRED argos3_simulator)
set(ARGOS_PREFIX ${ARGOS_PREFIX} CACHE INTERNAL "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)

# Find Lua
if (NOT ${CLUSTER})
  find_package(Lua52 REQUIRED)
endif()

# Set ARGoS include dir
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS} ${LUA_INCLUDE_DIR})

# Set ARGoS link dir
link_directories(${ARGOS_LIBRARY_DIRS})

# Descend into the controllers directory
add_subdirectory(controllers)

# Descend into the loop_functions directory
add_subdirectory(loop_functions)

# Descend into the embedding directory
add_subdirectory(embedding)
