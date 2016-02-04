list(APPEND src_files
    system/cmd_arg_reader.cpp
    system/path.cpp
    system/ui.cpp
    thread/QueueStage.cpp
    thread/tlogDefs.cpp
    mhd.cpp
    file/nrrd.cpp
    )

file(GLOB_RECURSE header_files "*.h")

include_directories(.)

add_library(${PROJECT_NAME} ${src_files})

set_target_properties(${PROJECT_NAME} PROPERTIES
    PROFILE_OUTPUT_NAME "${PROJECT_NAME}p"
    DEBUG_OUTPUT_NAME "${PROJECT_NAME}d"
    RELEASE_OUTPUT_NAME "${PROJECT_NAME}"
    )


# install
install(
    TARGETS     ${PROJECT_NAME} 
    DESTINATION lib/${PROJECT_NAME})

install (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h")

