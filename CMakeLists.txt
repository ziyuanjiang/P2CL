cmake_minimum_required(VERSION 3.1)

project(P2CL C CXX)
set (CMAKE_CXX_STANDARD 11)
message(STATUS "Project name            : ${CMAKE_PROJECT_NAME}")
message(STATUS "Project directory       : ${PROJECT_SOURCE_DIR}")

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
endif(NOT CMAKE_BUILD_TYPE)

#set addtional flags for Release version and Debug version
if(CMAKE_BUILD_TYPE MATCHES "Release")
    message("-- Building type: RELEASE")
else()
    message("-- Building type: DEBUG")
endif()

#Output directories
set(LIBRARY_OUTPUT_PATH     ${PROJECT_BINARY_DIR}/libs)
set(ARCHIVE_OUTPUT_PATH     ${PROJECT_BINARY_DIR}/libs)
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/bin)

#display tools and flags
message(STATUS "CMAKE_CXX_COMPILER:${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_RANLIB:${CMAKE_RANLIB}")
message(STATUS "CMAKE_AR:${CMAKE_AR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

include_directories($ENV{INTELOCLSDKROOT}/include)

message(STATUS "Building Software")


if(CMAKE_SYSTEM_NAME MATCHES Android)
	link_directories($ENV{HOME})
else()
	link_directories($ENV{INTELOCLSDKROOT}/lib/x64)
endif()

add_definitions(-DDEVICE=CL_DEVICE_TYPE_GPU)

add_subdirectory(skeletons)
add_subdirectory(frontend)
add_subdirectory(analysis)
add_subdirectory(runtime)
add_subdirectory(api)
add_library(p2cl $<TARGET_OBJECTS:skeletons> $<TARGET_OBJECTS:frontend> $<TARGET_OBJECTS:analysis> $<TARGET_OBJECTS:runtime> $<TARGET_OBJECTS:api>)

add_subdirectory(examples)

add_subdirectory(evaluation)
