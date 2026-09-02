cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)

add_library(character_framework SHARED
    character_framework.cpp safetyhook.cpp Zydis.c GGFramework.cpp
)
target_include_directories(character_framework PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(character_framework PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/character_framework
    PREFIX ""
    OUTPUT_NAME "character_framework"
)
target_link_options(character_framework PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/character_framework/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/character_framework/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying character_framework config.json"
)
add_custom_target(copy_character_framework_config ALL
    DEPENDS ${BIN_DIR}/character_framework/config.json
)
