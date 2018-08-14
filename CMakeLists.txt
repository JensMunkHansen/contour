cmake_minimum_required(VERSION 3.0)

project(contour VERSION 1.0.0.0 LANGUAGES C CXX)

# JEM: Allow the developer to select if Dynamic or Static libraries are built
option (CONTOUR_BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
set(LIB_TYPE STATIC)
if (CONTOUR_BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
endif()

if (CONTOUR_BUILD_SHARED_LIBS)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
endif()

# set up include-directories
include_directories(
  "${PROJECT_BINARY_DIR}"   # to find contour/config.h
  "${PROJECT_SOURCE_DIR}"   # to find contour/contour.hpp
  )

# Add sub-directories
add_subdirectory(contour)
