# For now, just Linux x64
set(DOTNET_RID "linux-x64")

find_program(DOTNET_EXECUTABLE dotnet)
if(NOT DOTNET_EXECUTABLE)
  message(WARNING "dotnet not found")
endif()

# Function to create a .NET project that wraps a native library
# Usage:
#   add_dotnet_library(
#     NAME MyLibrary
#     NATIVE_TARGETS target1 target2 ...
#     SOURCES Source1.cs Source2.cs
#   )
# Note: All targets in NATIVE_TARGETS will have PREFIX "" set for cross-platform DllImport compatibility
#       Runtime dependencies are automatically found using file(GET_RUNTIME_DEPENDENCIES)
function(add_dotnet_library)
  cmake_parse_arguments(DOTNET "" "NAME" "NATIVE_TARGETS;SOURCES" ${ARGN})

  # Set PREFIX "" on all native targets for cross-platform P/Invoke compatibility
  foreach(TARGET ${DOTNET_NATIVE_TARGETS})
    set_target_properties(${TARGET} PROPERTIES PREFIX "")
  endforeach()

  # Use first target as the primary P/Invoke target
  list(GET DOTNET_NATIVE_TARGETS 0 DOTNET_NATIVE_TARGET)

  set(DOTNET_PROJECT_DIR "${PROJECT_BINARY_DIR}/dotnet/${DOTNET_NAME}")
  set(DOTNET_NATIVE_DIR "${DOTNET_PROJECT_DIR}/native")

  # Generate CMake script to collect runtime dependencies at build time
  file(WRITE ${DOTNET_PROJECT_DIR}/collect_deps.cmake
"# Collect runtime dependencies for ${DOTNET_NAME}
cmake_minimum_required(VERSION 3.16)

set(NATIVE_LIB \"\${NATIVE_LIB_PATH}\")
set(OUTPUT_DIR \"\${OUTPUT_DIR}\")

# Create output directory
file(MAKE_DIRECTORY \${OUTPUT_DIR})

# Get runtime dependencies using CMake's built-in mechanism
file(GET_RUNTIME_DEPENDENCIES
  LIBRARIES \${NATIVE_LIB}
  RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPS
  UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
  POST_EXCLUDE_REGEXES \"^/lib/\" \"^/usr/lib\" \"^/lib64/\" \"^/usr/lib64/\"
)

# Copy main library
get_filename_component(LIB_NAME \${NATIVE_LIB} NAME)
file(COPY \${NATIVE_LIB} DESTINATION \${OUTPUT_DIR})
message(STATUS \"Copied: \${LIB_NAME}\")

# Copy resolved dependencies (project libraries only)
foreach(DEP \${RESOLVED_DEPS})
  get_filename_component(DEP_NAME \${DEP} NAME)
  file(COPY \${DEP} DESTINATION \${OUTPUT_DIR})
  message(STATUS \"Copied dependency: \${DEP_NAME}\")
endforeach()

if(UNRESOLVED_DEPS)
  message(STATUS \"Unresolved (system libs, ignored): \${UNRESOLVED_DEPS}\")
endif()

# Generate file list for .csproj
file(GLOB ALL_NATIVE_LIBS \${OUTPUT_DIR}/*.so \${OUTPUT_DIR}/*.dll \${OUTPUT_DIR}/*.dylib)
string(REPLACE \";\" \"\\n\" LIB_LIST \"\${ALL_NATIVE_LIBS}\")
file(WRITE \${OUTPUT_DIR}/libs.txt \"\${ALL_NATIVE_LIBS}\")
")

  # Native staging directory path for .csproj
  set(DOTNET_NATIVE_STAGING "${DOTNET_NATIVE_DIR}/$<CONFIG>")

  # Generate .csproj from template
  set(DOTNET_PACKAGE_ID ${DOTNET_NAME})
  set(DOTNET_NATIVE_TARGET ${DOTNET_NATIVE_TARGET})

  configure_file(
    ${PROJECT_SOURCE_DIR}/dotnet/${DOTNET_NAME}.csproj.in
    ${DOTNET_PROJECT_DIR}/${DOTNET_NAME}.csproj.in
    @ONLY)

  file(GENERATE
    OUTPUT ${DOTNET_PROJECT_DIR}/$<CONFIG>/${DOTNET_NAME}.csproj
    INPUT ${DOTNET_PROJECT_DIR}/${DOTNET_NAME}.csproj.in)

  # Copy C# sources to each config directory
  foreach(SOURCE ${DOTNET_SOURCES})
    file(GENERATE
      OUTPUT ${DOTNET_PROJECT_DIR}/$<CONFIG>/${SOURCE}
      INPUT ${PROJECT_SOURCE_DIR}/dotnet/${SOURCE})
  endforeach()

  # Target to collect native dependencies using file(GET_RUNTIME_DEPENDENCIES)
  add_custom_target(${DOTNET_NAME}_collect_deps
    COMMAND ${CMAKE_COMMAND}
      -DNATIVE_LIB_PATH=$<TARGET_FILE:${DOTNET_NATIVE_TARGET}>
      -DOUTPUT_DIR=${DOTNET_NATIVE_DIR}/$<CONFIG>
      -P ${DOTNET_PROJECT_DIR}/collect_deps.cmake
    DEPENDS ${DOTNET_NATIVE_TARGET}
    COMMENT "Collecting native dependencies for ${DOTNET_NAME}"
  )

  # Target to build the .NET library
  add_custom_target(${DOTNET_NAME}_dotnet_build
    COMMAND ${DOTNET_EXECUTABLE} build -c $<CONFIG> ${DOTNET_NAME}.csproj
    DEPENDS ${DOTNET_NAME}_collect_deps
    WORKING_DIRECTORY ${DOTNET_PROJECT_DIR}/$<CONFIG>
    COMMENT "Building ${DOTNET_NAME} .NET library"
  )

  # Target to create NuGet package
  add_custom_target(${DOTNET_NAME}_dotnet_pack
    COMMAND ${DOTNET_EXECUTABLE} pack -c $<CONFIG> ${DOTNET_NAME}.csproj
    DEPENDS ${DOTNET_NAME}_dotnet_build
    WORKING_DIRECTORY ${DOTNET_PROJECT_DIR}/$<CONFIG>
    COMMENT "Creating ${DOTNET_NAME} NuGet package"
  )
endfunction()

# Function to create a .NET test that references a local NuGet package
# Usage:
#   add_dotnet_test(
#     NAME TestName
#     SOURCE test.cs
#     PACKAGE_DIR /path/to/nuget/packages
#     PACKAGE_NAME Contour
#     DEPENDS Contour_dotnet_pack
#   )
function(add_dotnet_test)
  cmake_parse_arguments(TEST "" "NAME;SOURCE;PACKAGE_DIR;PACKAGE_NAME;DEPENDS" "" ${ARGN})

  set(TEST_PROJECT_DIR "${PROJECT_BINARY_DIR}/dotnet/tests/${TEST_NAME}")

  add_custom_target(${TEST_NAME}_create
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_PROJECT_DIR}
    COMMAND ${DOTNET_EXECUTABLE} new console --force --framework net8.0 --output ${TEST_PROJECT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${TEST_SOURCE} ${TEST_PROJECT_DIR}/Program.cs
    COMMAND ${DOTNET_EXECUTABLE} add ${TEST_PROJECT_DIR} package ${TEST_PACKAGE_NAME} --source ${TEST_PACKAGE_DIR}/$<CONFIG>/bin/$<CONFIG>
    DEPENDS ${TEST_DEPENDS}
    COMMENT "Creating test project ${TEST_NAME}"
  )

  add_custom_target(${TEST_NAME}_build
    COMMAND ${DOTNET_EXECUTABLE} build ${TEST_PROJECT_DIR} -c $<CONFIG>
    DEPENDS ${TEST_NAME}_create
    COMMENT "Building ${TEST_NAME}"
  )

  add_custom_target(${TEST_NAME}_run
    COMMAND ${DOTNET_EXECUTABLE} run --project ${TEST_PROJECT_DIR} -c $<CONFIG>
    DEPENDS ${TEST_NAME}_build
    COMMENT "Running ${TEST_NAME}"
  )

  # Register with CTest
  add_test(
    NAME ${TEST_NAME}
    COMMAND ${DOTNET_EXECUTABLE} run --project ${TEST_PROJECT_DIR} -c $<CONFIG>
  )
endfunction()
