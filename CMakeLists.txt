cmake_minimum_required(VERSION 3.10)

project(contour
  LANGUAGES C CXX
  VERSION 1.0.0.0)

set(project_policies CMP0078 CMP0086)
foreach(policy ${project_policies})
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()

if (NOT DEFINED ENABLE_PYTHON3)
  set(ENABLE_PYTHON3 ON)
endif()

# JEM: Allow the developer to select if Dynamic or Static libraries are built
option (CONTOUR_BUILD_SHARED_LIBS "Build Shared Libraries" ON)
set(LIB_TYPE STATIC)
if (CONTOUR_BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
endif()

if (CONTOUR_BUILD_SHARED_LIBS)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
endif()

# Add sub-directories
add_subdirectory(contour)
