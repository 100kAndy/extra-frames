cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(ExtraFrames VERSION 2.0.0)

file(GLOB SOURCES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
	message(FATAL_ERROR "Unable to find Geode SDK! Set the GEODE_SDK environment variable.")
endif()

# Build the code generator from the same bindings source it reads, instead of
# downloading a prebuilt one that can be out of sync with the bindings.
set(SKIP_BUILDING_CODEGEN OFF CACHE BOOL "Build codegen locally" FORCE)

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})
