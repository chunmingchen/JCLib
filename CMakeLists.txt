# JCLib v0.1
project (jclib)
cmake_minimum_required (VERSION 2.6)

# settings
SET(CMAKE_BUILD_TYPE Debug CACHE STRING "default to debug")

# debug type settings
IF(CMAKE_BUILD_TYPE MATCHES DEBUG) 
    message("debug mode") 
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG) 

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# more flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++" )

# platform settings
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#	add_definitions( -stdlib=libc++ )
ENDIF()


# directories

INCLUDE_DIRECTORIES(.)

list(APPEND SRC_FILES
	system/cmd_arg_reader.cpp
	system/path.cpp
	thread/QueueStage.cpp
	thread/tlogDefs.cpp
	)

add_library(${PROJECT_NAME} ${SRC_FILES})


# output

set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_OUTPUT_NAME "${PROJECT_NAME}d"
	RELEASE_OUTPUT_NAME "${PROJECT_NAME}"
	)

install(
	TARGETS 	${PROJECT_NAME} 
	DESTINATION	.)	

