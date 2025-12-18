cmake_minimum_required(VERSION 3.14...3.28)

project(contour
  LANGUAGES C CXX
  VERSION 1.0.0.0)

# Options
option(CONTOUR_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(CONTOUR_BUILD_PYTHON "Build Python bindings" ON)
option(CONTOUR_BUILD_DOTNET "Build .NET bindings" OFF)

# PIC needed for Python module (shared library)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

# Add sub-directories
add_subdirectory(contour)

# .NET bindings (requires shared library)
if(CONTOUR_BUILD_DOTNET)
  if(NOT CONTOUR_BUILD_SHARED_LIBS)
    message(FATAL_ERROR "CONTOUR_BUILD_DOTNET requires CONTOUR_BUILD_SHARED_LIBS=ON")
  endif()
  include(cmake/dotnet.cmake)

  add_dotnet_library(
    NAME Contour
    NATIVE_TARGETS contour
    SOURCES Contour.cs
  )

  add_dotnet_test(
    NAME ContourTest
    SOURCE ${PROJECT_SOURCE_DIR}/dotnettest/ContourTest.cs
    PACKAGE_DIR ${PROJECT_BINARY_DIR}/dotnet/Contour
    PACKAGE_NAME Contour
    DEPENDS Contour_dotnet_pack
  )
endif()
