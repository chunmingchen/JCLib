# JCLib v0.1
project (jclib)
cmake_minimum_required (VERSION 2.6)

# settings

##############################################
# defaults for cmake_build_type
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "DEBUG;RELEASE;PROFILE")
option(use_gprof "Use GProf profiling" OFF)

# debug type settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -D_PROFILE")

# platform settings
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	add_definitions( -stdlib=libc++ )
ENDIF()


####################################
# Profiling
if (use_gprof)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif (use_gprof)

####################################
add_subdirectory (src)

####################################
