cmake_minimum_required(VERSION 3.10)


project(zookeeper_test)

add_definitions(-DTHREADED)

find_package(zookeeper REQUIRED)


add_executable(${PROJECT_NAME} main.cpp ../zkutil.cpp ../zkutil.h)
target_link_libraries(${PROJECT_NAME}  zookeeper::zookeeper)

#设置属性
set_target_properties(${PROJECT_NAME} PROPERTIES 
	COMPILE_FLAGS	"-pthread"
	LINK_FLAGS "-pthread")
