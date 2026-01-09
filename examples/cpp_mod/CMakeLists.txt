cmake_minimum_required(VERSION 3.21)


add_library(example_cpp_mod SHARED
    example_cpp_mod.cpp
)
target_include_directories(example_cpp_mod PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(example_cpp_mod PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/example_cpp_mod
    PREFIX ""
    OUTPUT_NAME "example_cpp_mod"
)
target_link_options(example_cpp_mod PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/example_cpp_mod/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/example_cpp_mod/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying example_cpp_mod config.json"
)
add_custom_target(copy_cpp_example_config ALL
    DEPENDS ${BIN_DIR}/example_cpp_mod/config.json
)
