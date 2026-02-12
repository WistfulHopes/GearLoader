cmake_minimum_required(VERSION 3.21)


add_library(baseModTester SHARED
    baseModTester.cpp
)
target_include_directories(baseModTester PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(baseModTester PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/baseModTester
    PREFIX ""
    OUTPUT_NAME "baseModTester"
)
target_link_options(baseModTester PRIVATE
    -static
)

add_custom_command(
    OUTPUT ${BIN_DIR}/baseModTester/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/baseModTester/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying baseModTester config.json"
)
add_custom_target(copy_baseModTester_config ALL
    DEPENDS ${BIN_DIR}/baseModTester/config.json
)