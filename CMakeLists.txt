cmake_minimum_required (VERSION 3.12)
project (Racetrack)

# find all source and header files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.hpp")

add_executable(racetrack ${SOURCE_FILES})
set_target_properties(racetrack PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

add_library(lodepng STATIC "dependencies/lodepng/lodepng.cpp")
target_include_directories(lodepng PUBLIC dependencies/lodepng)
target_link_libraries(racetrack PRIVATE lodepng)

#find_package(SFML COMPONENTS system graphics window REQUIRED)
#target_link_libraries(racetrack PRIVATE sfml-system sfml-graphics sfml-window)
