cmake_minimum_required(VERSION 3.14...3.28)

project(contour
  LANGUAGES C CXX
  VERSION 1.0.0.0)

# Options
option(CONTOUR_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(CONTOUR_BUILD_PYTHON "Build Python bindings" ON)

# PIC needed for Python module (shared library)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add sub-directories
add_subdirectory(contour)
