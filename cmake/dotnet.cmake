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
#     NATIVE_TARGET mytarget
#     SOURCES Source1.cs Source2.cs
#   )
function(add_dotnet_library)
  cmake_parse_arguments(DOTNET "" "NAME;NATIVE_TARGET" "SOURCES" ${ARGN})

  set(DOTNET_PROJECT_DIR "${PROJECT_BINARY_DIR}/dotnet/${DOTNET_NAME}")
  file(MAKE_DIRECTORY ${DOTNET_PROJECT_DIR})

  # Copy C# sources
  foreach(SOURCE ${DOTNET_SOURCES})
    configure_file(
      ${PROJECT_SOURCE_DIR}/dotnet/${SOURCE}
      ${DOTNET_PROJECT_DIR}/${SOURCE}
      COPYONLY)
  endforeach()

  # Generate .csproj from template
  set(DOTNET_PACKAGE_ID ${DOTNET_NAME})
  set(DOTNET_NATIVE_TARGET ${DOTNET_NATIVE_TARGET})

  configure_file(
    ${PROJECT_SOURCE_DIR}/dotnet/${DOTNET_NAME}.csproj.in
    ${DOTNET_PROJECT_DIR}/${DOTNET_NAME}.csproj.in
    @ONLY)

  file(GENERATE
    OUTPUT ${DOTNET_PROJECT_DIR}/${DOTNET_NAME}.csproj
    INPUT ${DOTNET_PROJECT_DIR}/${DOTNET_NAME}.csproj.in)
endfunction()
