cmake_minimum_required(VERSION 2.8)

include_directories(${PROJECT_SOURCE_DIR}/skeletons)
include_directories(${PROJECT_SOURCE_DIR}/frontend)
include_directories(${PROJECT_SOURCE_DIR}/analysis)
include_directories(${PROJECT_SOURCE_DIR}/runtime)
include_directories(${PROJECT_SOURCE_DIR}/api)

file(GLOB_RECURSE API_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE API_HEADER
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

file(GLOB_RECURSE SKELETONS_HEADER
	"${PROJECT_SOURCE_DIR}/skeletons/*.h"
	"${PROJECT_SOURCE_DIR}/skeletons/*.hpp"
)

file(GLOB_RECURSE ANALYSIS_HEADER
	"${PROJECT_SOURCE_DIR}/analysis/*.h"
	"${PROJECT_SOURCE_DIR}/analysis/*.hpp"
)
file(GLOB_RECURSE FRONTEND_HEADER
	"${PROJECT_SOURCE_DIR}/frontend/*.h"
	"${PROJECT_SOURCE_DIR}/frontend/*.hpp"
)

file(GLOB_RECURSE RUNTIME_HEADER
	"${PROJECT_SOURCE_DIR}/runtime/*.h"
	"${PROJECT_SOURCE_DIR}/runtime/*.hpp"
)


add_library(api OBJECT ${API_SRC} ${API_HEADER} ${FRONTEND_HEADER}
	${ANALYSIS_HEADER} ${SKELETONS_HEADER} ${RUNTIME_HEADER})

