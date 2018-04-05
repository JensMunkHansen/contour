include(GenerateExportHeader)
set(conrec_SOURCES
  conrec.c
)

add_library(conrec ${LIB_TYPE} ${conrec_SOURCES})

if (UNIX)
  set(CMAKE_CXX_FLAGS "-std=c++11 -std=c++14 -pedantic -Wall ${CMAKE_CXX_FLAGS} -std=c++14")
endif()

set(contour_SOURCES
  contour.cpp
)

set(contour_HEADERS
)

if (UNIX AND NOT WIN32 AND NOT CYGWIN)
  set(contour_HEADERS "${contour_HEADERS}" "../sps/strace.hpp")
  set(contour_SOURCES "${contour_SOURCES}" "../sps/strace.cpp")
endif()

add_library(contour ${LIB_TYPE} ${contour_SOURCES})

generate_export_header(contour)

target_link_libraries(contour conrec)

find_package(SWIG REQUIRED)

if (UNIX AND NOT CYGWIN)
  set(SWIG_EXECUTABLE swig3.0)
endif()

include(${SWIG_USE_FILE})

find_package(PythonInterp 2.7 REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)

include_directories(${PYTHON_INCLUDE_PATH})

set(swig_contour_HEADERS
  contour.hpp
)

set_source_files_properties(swig_contour.i PROPERTIES CPLUSPLUS ON)

swig_add_module(swig_contour python swig_contour.i ${swig_contour_HEADERS})

swig_link_libraries(swig_contour contour conrec ${PYTHON_LIBRARIES})
