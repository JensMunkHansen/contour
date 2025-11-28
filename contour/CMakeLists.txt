include(GenerateExportHeader)

# Determine library type
if(CONTOUR_BUILD_SHARED_LIBS)
  set(CONTOUR_LIB_TYPE SHARED)
else()
  set(CONTOUR_LIB_TYPE STATIC)
endif()

# conrec library (C) - internal implementation
add_library(conrec ${CONTOUR_LIB_TYPE}
  conrec.c
  conrec.h
)
set_target_properties(conrec PROPERTIES
  C_STANDARD 99
  C_STANDARD_REQUIRED ON
)

# Generate config.h
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(SPS_DEBUG 1)
endif()
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
  @ONLY
)

# contour library (C++)
add_library(contour ${CONTOUR_LIB_TYPE}
  contour.cpp
  contour.hpp
)

target_compile_definitions(contour PRIVATE USE_CMAKE)

target_include_directories(contour
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
)

target_link_libraries(contour PRIVATE conrec)

target_compile_features(contour PUBLIC cxx_std_14)
set_target_properties(contour PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

generate_export_header(contour)

# Alias for use as subproject
add_library(contour::contour ALIAS contour)

# Compiler warnings (applied to our code only, not SWIG-generated)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(conrec PRIVATE -Wall -Wextra -pedantic)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(contour PRIVATE -Wall -Wextra -pedantic)
endif()

# Python bindings
if(CONTOUR_BUILD_PYTHON)
  find_package(SWIG 4.0 COMPONENTS python)
  find_package(Python3 COMPONENTS Interpreter Development NumPy)

  if(SWIG_FOUND AND Python3_FOUND AND Python3_NumPy_FOUND)
    message(STATUS "Building Python bindings for contour")
    include(UseSWIG)

    set_source_files_properties(swig_contour.i PROPERTIES
      CPLUSPLUS ON
      SWIG_MODULE_NAME swig_contour
      INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/.."
    )

    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.20)
      set_property(SOURCE swig_contour.i PROPERTY USE_SWIG_DEPENDENCIES TRUE)
    endif()

    if(MSVC)
      set_property(SOURCE swig_contour.i PROPERTY SWIG_FLAGS "-D_SWIG_WIN32")
    endif()

    swig_add_library(swig_contour
      LANGUAGE python
      SOURCES swig_contour.i contour.hpp
    )

    target_include_directories(swig_contour PRIVATE
      ${Python3_INCLUDE_DIRS}
      ${Python3_NumPy_INCLUDE_DIRS}
    )

    swig_link_libraries(swig_contour
      contour
      Python3::Python
    )

    # Suppress warnings in SWIG-generated code
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
      target_compile_options(swig_contour PRIVATE
        -Wno-deprecated-declarations
        -Wno-unused-parameter
        -Wno-missing-field-initializers
      )
    elseif(MSVC)
      target_compile_definitions(swig_contour PRIVATE
        SWIG_PYTHON_INTERPRETER_NO_DEBUG
        HAVE_ROUND
      )
      target_compile_options(swig_contour PRIVATE
        /wd6011 /wd6244 /wd6246 /wd4459 /wd4701 /wd4127 /wd4456
      )
    endif()

    if(MSVC)
      add_custom_command(TARGET swig_contour POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE:swig_contour>
          "${CMAKE_CURRENT_BINARY_DIR}"
      )
    endif()
  else()
    message(STATUS "Python bindings disabled: SWIG=${SWIG_FOUND}, Python3=${Python3_FOUND}, NumPy=${Python3_NumPy_FOUND}")
  endif()
endif()
