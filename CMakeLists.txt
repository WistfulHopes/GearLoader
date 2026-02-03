cmake_minimum_required(VERSION 3.21)
project(GearLoader
    VERSION 2.0.0
    DESCRIPTION "Mod Loader for GGXXACPR"
    LANGUAGES
        CXX
        C
)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")

add_subdirectory(examples/c_mod)
add_subdirectory(examples/cpp_mod)
add_subdirectory(source/baseMod)

# util object files
add_library(util OBJECT)

target_include_directories(util PRIVATE
    include
    third_party
    source/modLoader
)
target_sources(util
    PRIVATE
        source/modLoader/common/versionParsing.cpp
        source/modLoader/dependencyManager/configParser.cpp
        source/modLoader/dependencyManager/dependencyManager.cpp
        source/modLoader/gearLoaderApi/gearLoader.cpp
)

target_include_directories(util
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Main mod loader dll
add_library(GearLoaderDll SHARED
    source/modLoader/source.cpp
    source/modLoader/exports.def
)
target_include_directories(GearLoaderDll PRIVATE
    include
    third_party
    source/modLoader
)
target_sources(GearLoaderDll
    PRIVATE
        $<TARGET_OBJECTS:util>
)
target_compile_definitions(GearLoaderDll
    PRIVATE exports
)
set_target_properties(GearLoaderDll PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/GearLoader
    PREFIX ""
    OUTPUT_NAME "DBGHELP"
)
target_link_options(GearLoaderDll PRIVATE
    -static
)

# Unit testing exe
add_executable(Tests
    test/test.cpp
)
target_include_directories(Tests PRIVATE
    include
    third_party
    source/modLoader
)
target_sources(Tests
    PRIVATE
        $<TARGET_OBJECTS:util>
)
set_target_properties(Tests PROPERTIES
    OUTPUT_NAME "RunTests"
)
target_link_options(Tests PRIVATE
    -static
)
