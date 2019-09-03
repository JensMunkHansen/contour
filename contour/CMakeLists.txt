include(GenerateExportHeader)

set(conrec_SOURCES
  conrec.c
)

add_definitions(-DUSE_CMAKE)

add_library(conrec ${LIB_TYPE} ${conrec_SOURCES})

if (UNIX)
  set(CMAKE_CXX_FLAGS "-std=c++14 -pedantic -Wall ${CMAKE_CXX_FLAGS}")
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

set(contour_SOURCES
  contour.cpp
)

set(contour_HEADERS
)

if (UNIX AND NOT WIN32 AND NOT CYGWIN AND SPS_DEBUG)
  set(contour_HEADERS "${contour_HEADERS}" "../sps/strace.hpp")
  set(contour_SOURCES "${contour_SOURCES}" "../sps/strace.cpp")
endif()

if (MSVC)
   set_source_files_properties( contour.cpp PROPERTIES COMPILE_FLAGS "/wd6011 /wd6386")
endif()

add_library(contour ${LIB_TYPE} ${contour_SOURCES})

generate_export_header(contour)

target_link_libraries(contour conrec)

find_package(SWIG REQUIRED)

include(${SWIG_USE_FILE})

find_package(PythonInterp 2.7 REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)

if (MSVC)
  set(CMAKE_SWIG_FLAGS "-D_SWIG_WIN32")
  add_definitions(-DSWIG_PYTHON_INTERPRETER_NO_DEBUG)
  add_definitions(-DHAVE_ROUND)
  string(REPLACE "_d" "" PYTHON_LIBRARIES "${PYTHON_LIBRARIES}")
endif()

include_directories(${PYTHON_INCLUDE_PATH})

set(swig_contour_HEADERS
  contour.hpp
)

set_source_files_properties(swig_contour.i PROPERTIES CPLUSPLUS ON)

if(${CMAKE_VERSION} VERSION_LESS "3.8.0")
  swig_add_module(swig_contour python swig_contour.i ${swig_contour_HEADERS})
else()
  swig_add_library(swig_contour LANGUAGE python SOURCES swig_contour.i ${swig_contour_HEADERS})
endif()

if (MSVC)
  # Setting RUNTIME_OUTPUT_DIRECTORY using set_target_properties does not work
  add_custom_command(TARGET _swig_contour POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:_swig_contour> "${CMAKE_CURRENT_BINARY_DIR}"
    )
endif()

if (MSVC)
  # 4459: declaration of swig_this hides global declaration,
  #       e.g. static PyObject* swig_this = NULL;
  #       void SomeFunction(PyObject* swig_this);
  set_source_files_properties( ${swig_generated_file_fullname} PROPERTIES COMPILE_FLAGS "/wd6011 /wd6244 /wd6246 /wd4459 /wd4701 /wd4127 /wd4456 /Od")
endif()

swig_link_libraries(swig_contour contour conrec ${PYTHON_LIBRARIES})
