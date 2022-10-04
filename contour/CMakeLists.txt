include(GenerateExportHeader)

# Definition used to allow compilation using python build utilities
add_definitions(-DUSE_CMAKE)

add_library(conrec ${LIB_TYPE}
  conrec.c
)

if (UNIX)
  set(CMAKE_CXX_FLAGS "-pedantic -Wall ${CMAKE_CXX_FLAGS}")
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

if (MSVC)
  set_source_files_properties(contour.cpp PROPERTIES COMPILE_FLAGS "/wd6011 /wd6386")
endif()

add_library(contour ${LIB_TYPE} contour.cpp)

target_include_directories(contour
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.."
  PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/..")

generate_export_header(contour)

target_link_libraries(contour PRIVATE conrec)

target_compile_features(contour PRIVATE cxx_std_14)

find_package(SWIG)

add_library(libs::contour ALIAS contour)

include(${SWIG_USE_FILE})

if (WIN32)
  find_package(Python3 COMPONENTS Interpreter Development NumPy)
else()
  # On linux, we require a specific version of Python
  find_package(Python3 3.9 COMPONENTS Interpreter Development NumPy)
endif()
include_directories(${Python3_INCLUDE_DIRS})
include_directories(${Python3_NumPy_INCLUDE_DIRS})

if (MSVC)
  set(CMAKE_SWIG_FLAGS "-D_SWIG_WIN32")
  add_definitions(-DSWIG_PYTHON_INTERPRETER_NO_DEBUG)
  add_definitions(-DHAVE_ROUND)
  string(REPLACE "_d" "" PYTHON_LIBRARIES "${PYTHON_LIBRARIES}")
endif()

include_directories(${PYTHON_INCLUDE_DIRS})

set(swig_contour_HEADERS
  contour.hpp
)

set_source_files_properties(swig_contour.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE swig_contour.i PROPERTY SWIG_MODULE_NAME swig_contour) # Explicitly set name -module is passed to SWIG
set_property(SOURCE swig_contour.i PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/..")
swig_add_library(swig_contour LANGUAGE python SOURCES swig_contour.i ${swig_contour_HEADERS})

if (MSVC)
  # Setting RUNTIME_OUTPUT_DIRECTORY using set_target_properties does not work
  add_custom_command(TARGET ${SWIG_MODULE_swig_contour_REAL_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SWIG_MODULE_swig_contour_REAL_NAME}> "${CMAKE_CURRENT_BINARY_DIR}"
  )

  # 4459: declaration of swig_this hides global declaration,
  #       e.g. static PyObject* swig_this = NULL;
  #       void SomeFunction(PyObject* swig_this);
  set_source_files_properties( ${swig_generated_file_fullname} PROPERTIES COMPILE_FLAGS "/wd6011 /wd6244 /wd6246 /wd4459 /wd4701 /wd4127 /wd4456 /Od")
endif()

#swig_link_libraries(swig_contour contour conrec ${PYTHON_LIBRARIES})
target_link_libraries(${SWIG_MODULE_swig_contour_REAL_NAME} contour conrec ${Python3_LIBRARIES})
