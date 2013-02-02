# JCLib v0.1
project (jclib)
cmake_minimum_required (VERSION 2.6)

# settings

##############################################
# defaults for cmake_build_type
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "DEBUG;RELEASE;PROFILE")


# debug type settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -D_PROFILE")


# platform settings
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	add_definitions( -stdlib=libc++ )
ENDIF()


####################################

# directories

INCLUDE_DIRECTORIES(.)

list(APPEND SRC_FILES
	system/cmd_arg_reader.cpp
	system/path.cpp
	thread/QueueStage.cpp
	thread/tlogDefs.cpp
	mhd.cpp
	)

add_library(${PROJECT_NAME} ${SRC_FILES})



# output
set_target_properties(${PROJECT_NAME} PROPERTIES
	PROFILE_OUTPUT_NAME "${PROJECT_NAME}p"
        DEBUG_OUTPUT_NAME "${PROJECT_NAME}d"
        RELEASE_OUTPUT_NAME "${PROJECT_NAME}"
        )

install(
	TARGETS 	${PROJECT_NAME} 
	DESTINATION	.)	

