cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 20)

set (CMAKE_EXE_LINKER_FLAGS -pthread)

# set the project name
project(ASE VERSION 1.0)

include_directories(include)
include_directories(../shared/include)

file(GLOB allfiles
     "src/*.cpp"
     "../shared/src/*.cpp"
)

add_compile_definitions(DEBUG)
set(CMAKE_BUILD_TYPE Debug)

# add the executable
add_executable(server ${allfiles})
