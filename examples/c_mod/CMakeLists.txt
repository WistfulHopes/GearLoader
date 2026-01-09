cmake_minimum_required(VERSION 3.21)


add_library(example_c_mod SHARED
    example_c_mod.c
)
target_include_directories(example_c_mod PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(example_c_mod PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/example_c_mod
    PREFIX ""
    OUTPUT_NAME "example_c_mod"
)
target_link_options(example_c_mod PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/example_c_mod/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/example_c_mod/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying example_c_mod config.json"
)
add_custom_target(copy_c_example_config ALL
    DEPENDS ${BIN_DIR}/example_c_mod/config.json
)
