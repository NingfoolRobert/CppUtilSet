cmake_minimum_required(VERSION 3.10)

project(redis_t)


set(TARGET_NAME  redis_test)

find_package(hiredis REQUIRED)


add_executable(${TARGET_NAME}
	main.cpp 
	../redisutil.h 
	../redisutil.cpp
	../redis_sentinel_util.h 
	../redis_sentinel_util.cpp)

target_link_libraries(${TARGET_NAME} PRIVATE  hiredis::hiredis_static)

set_target_properties(${TARGET_NAME} PROPERTIES 
	COMPILE_FLAGS  "-pthread"
	LINK_FLAGS "-pthread")

set_target_properties(${TARGET_NAME} PROPERTIES 
	WINDOWS_EXPORT_ALL_SYMBOLS True)

