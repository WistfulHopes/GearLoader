cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/angelscript/projects/cmake)

add_library(rescript SHARED
    src/rescript.cpp src/safetyhook.cpp src/Zydis.c src/rescript_as.cpp angelscript/add_on/scriptstdstring/scriptstdstring.cpp
        angelscript/add_on/scriptbuilder/scriptbuilder.cpp
)
target_include_directories(rescript PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/angelscript/add_on
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/mods/character_framework
)
set_target_properties(rescript PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/rescript
    PREFIX ""
    OUTPUT_NAME "rescript"
)
target_link_options(rescript PRIVATE
    -static
)
target_link_libraries(rescript PRIVATE ${ANGELSCRIPT_LIBRARY_NAME})

add_custom_command(
    OUTPUT ${BIN_DIR}/rescript/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${BIN_DIR}/rescript/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    COMMENT "Copying rescript config.json"
)
add_custom_target(copy_rescript_config ALL
    DEPENDS ${BIN_DIR}/rescript/config.json
)
