cmake_minimum_required(VERSION 3.10)

set(TARGET_NAME redisutil)

add_library(${TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME}
	INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(${TARGET_NAME}
	INTERFACE 
	${CMAKE_CURRENT_SOURCE_DIR}/redisutil.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/redis_sentinel_util.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/redis_key_util.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/redis_hash_util.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/redis_mgr.cpp
	)

add_subdirectory(test)
