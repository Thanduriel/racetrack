cmake_minimum_required (VERSION 3.12)
project (Racetrack)

OPTION(WITH_GRAPHICS "Enable graphical interface." OFF)

# find all source and header files
file(GLOB_RECURSE SOURCE_FILES "src/main.cpp" "src/game/*.cpp" "src/math/*.cpp" "src/bots/*.cpp")

if (WITH_GRAPHICS)
    file(GLOB_RECURSE GRAPHICS_SOURCE_FILES "src/graphics/*.cpp" "src/graphics/*.hpp")
    list(APPEND SOURCE_FILES ${GRAPHICS_SOURCE_FILES})
endif ()

add_executable(racetrack ${SOURCE_FILES})
set_target_properties(racetrack PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    set(COMPILATION_OPTIONS 
        $<$<CONFIG:Debug>:-Wall;-pedantic>
        $<$<CONFIG:RelWithDebInfo>:-Wall;-pedantic>
        $<$<CONFIG:Release>:-Wall;-pedantic;-O3;-DNDEBUG;-march=native > 
        $<$<CONFIG:MinSizeRel>:-Wall;-pedantic>
    )
    target_compile_options( racetrack PUBLIC "${COMPILATION_OPTIONS}" )
endif()

add_library(lodepng STATIC "dependencies/lodepng/lodepng.cpp")
target_include_directories(lodepng PUBLIC dependencies/lodepng)
target_link_libraries(racetrack PRIVATE lodepng)

if (WITH_GRAPHICS)
    target_compile_definitions(racetrack PRIVATE "WITH_GRAPHICS")
    set(SFML_BUILD_NETWORK OFF)
    set(SFML_BUILD_AUDIO OFF)
    set(SFML_BUILD_EXAMPLES OFF)
    set(SFML_BUILD_DOC OFF)
    set(SFML_BUILD_TEST_SUITE OFF)
    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
        FIND_PACKAGE_ARGS 3.1 COMPONENTS System Window Graphics
    )
    FetchContent_MakeAvailable(SFML)
    target_link_libraries(racetrack PRIVATE SFML::Graphics SFML::System SFML::Window)
endif ()
