cmake_minimum_required(VERSION 2.8)


include_directories(${PROJECT_SOURCE_DIR}/skeletons)
include_directories(${PROJECT_SOURCE_DIR}/frontend)
include_directories(${PROJECT_SOURCE_DIR}/analysis)
include_directories(${PROJECT_SOURCE_DIR}/runtime)

file(GLOB_RECURSE ANALYSIS_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE ANALYSIS_HEADER
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

file(GLOB_RECURSE RUNTIME_HEADER
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

file(GLOB_RECURSE SKELETONS_HEADER
	"${PROJECT_SOURCE_DIR}/skeletons/*.h"
	"${PROJECT_SOURCE_DIR}/skeletons/*.hpp"
)

file(GLOB_RECURSE FRONTEND_HEADER
	"${PROJECT_SOURCE_DIR}/frontend/*.h"
	"${PROJECT_SOURCE_DIR}/frontend/*.hpp"
)

add_library(analysis OBJECT ${ANALYSIS_SRC} ${ANALYSIS_HEADER} ${SKELETONS_HEADER} 
		${FRONTEND_HEADER} ${RUNTIME_HEADER})
