cmake_minimum_required(VERSION 3.20)
project(ida-wii-loaders)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED IDASDK)
  if(DEFINED ENV{IDASDK})
    set(IDASDK "$ENV{IDASDK}")
  else()
    message(FATAL_ERROR "IDA SDK unset, pass -DIDASDK=/path/to/ida-sdk/src")
  endif()
endif()

message(STATUS "Using IDA SDK: ${IDASDK}")

if(NOT EXISTS "${IDASDK}/include/ida.hpp")
  message(FATAL_ERROR "Invalid IDASDK path: ${IDASDK}/include/ida.hpp not found")
endif()

set(IDA_INCLUDE_DIR "${IDASDK}/include")

if(APPLE)
  set(IDA_PLATFORM "__MAC__")
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64" OR CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
    set(IDA_LIB_DIR "${IDASDK}/lib/arm64_mac_clang_64")
  else()
    set(IDA_LIB_DIR "${IDASDK}/lib/x64_mac_clang_64")
  endif()
  set(IDA_LIB_NAME "ida")
  set(LOADER_EXTENSION ".dylib")
elseif(UNIX)
  set(IDA_PLATFORM "__LINUX__")
  set(IDA_LIB_DIR "${IDASDK}/lib/x64_linux_gcc_64")
  set(IDA_LIB_NAME "ida")
  set(LOADER_EXTENSION ".so")
elseif(WIN32)
  set(IDA_PLATFORM "__NT__")
  set(IDA_LIB_DIR "${IDASDK}/lib/x64_win_vc_64")
  set(IDA_LIB_NAME "ida")
  set(LOADER_EXTENSION ".dll")
endif()

find_library(IDA_LIB NAMES ${IDA_LIB_NAME} HINTS "${IDA_LIB_DIR}")
if(NOT IDA_LIB)
  message(FATAL_ERROR "Could not find IDA lib in ${IDA_LIB_DIR}")
endif()
message(STATUS "Found IDA lib: ${IDA_LIB}")

function(add_ida_loader target_name)
  cmake_parse_arguments(LOADER "" "" "SOURCES" ${ARGN})

  add_library(${target_name} SHARED ${LOADER_SOURCES})

  target_include_directories(${target_name} PRIVATE "${IDA_INCLUDE_DIR}")

  target_compile_definitions(${target_name} PRIVATE
    ${IDA_PLATFORM}
    __EA64__=1
    __X64__=1
    LOADER_COMPILE
  )

  if(NOT MSVC)
    target_compile_options(${target_name} PRIVATE
      -Wno-deprecated-declarations
      -Wno-nullability-completeness
      -Wno-nullability-extension
    )
  endif()

  target_link_libraries(${target_name} PRIVATE "${IDA_LIB}")

  set_target_properties(${target_name} PROPERTIES
    PREFIX ""
    SUFFIX "${LOADER_EXTENSION}"
  )

  if(APPLE)
    set_target_properties(${target_name} PROPERTIES
      MACOSX_RPATH TRUE
      BUILD_WITH_INSTALL_RPATH TRUE
      INSTALL_RPATH "@executable_path"
    )
  endif()
endfunction()

add_subdirectory(dol)
add_subdirectory(rel)
