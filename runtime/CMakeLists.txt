cmake_minimum_required(VERSION 2.8)

include_directories(${PROJECT_SOURCE_DIR}/skeletons)
include_directories(${PROJECT_SOURCE_DIR}/frontend)
#include_directories(${PROJECT_SOURCE_DIR}/analysis)
include_directories(${PROJECT_SOURCE_DIR}/runtime)


file(GLOB_RECURSE RUNTIME_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE RUNTIME_HEADER
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

file(GLOB_RECURSE SKELETONS_HEADER
	"${PROJECT_SOURCE_DIR}/skeletons/*.h"
	"${PROJECT_SOURCE_DIR}/skeletons/*.hpp"
)

file(GLOB_RECURSE RUNTIME_HEADER
	"${PROJECT_SOURCE_DIR}/runtime/*.h"
	"${PROJECT_SOURCE_DIR}/runtime/*.hpp"
)

add_library(runtime OBJECT ${RUNTIME_SRC} 
	${RUNTIME_HEADER} ${FRONTEND_HEADER} ${SKELETONS_HEADER})

