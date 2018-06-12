cmake_minimum_required(VERSION 2.8)


include_directories(${PROJECT_SOURCE_DIR}/skeletons)
include_directories(${PROJECT_SOURCE_DIR}/frontend)

file(GLOB_RECURSE SKELETONS_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE SKELETONS_HEADER
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(skeletons OBJECT ${SKELETONS_SRC} ${SKELETONS_HEADER} ${PROJECT_SOURCE_DIR}/frontend/exception.hpp ${PROJECT_SOURCE_DIR}/frontend/frontendutils.hpp)
