cmake_minimum_required(VERSION 3.0)

project(contour C CXX)

# JEM: Allow the developer to select if Dynamic or Static libraries are built
option (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
set(LIB_TYPE STATIC)
if (BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
endif()

if (BUILD_SHARED_LIBS)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
endif()

# set up include-directories
include_directories(
  "${PROJECT_BINARY_DIR}"   # to find fnm/config.h
  "${PROJECT_SOURCE_DIR}"   # to find fnm/fnm_types.hpp
  )

# Add sub-directories
add_subdirectory(contour)
