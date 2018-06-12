cmake_minimum_required(VERSION 2.8)

include_directories(${PROJECT_SOURCE_DIR}/skeletons)
include_directories(${PROJECT_SOURCE_DIR}/frontend)
include_directories(${PROJECT_SOURCE_DIR}/analysis)
include_directories(${PROJECT_SOURCE_DIR}/runtime)
include_directories(${PROJECT_SOURCE_DIR}/api)

file(GLOB_RECURSE TEST_CASES
	"${CMAKE_CURRENT_SOURCE_DIR}/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
find_package( Threads )

foreach(T ${TEST_CASES})
	string(REGEX REPLACE ".*/(.*).c[pp]*" "\\1" test_name ${T})

	add_executable(${test_name} ${T} )

	#set_target_properties(${test_name} PROPERTIES OUTPUT_NAME "${test_name}")

	target_link_libraries(${test_name} p2cl m OpenCL ${CMAKE_THREAD_LIBS_INIT})

endforeach()

