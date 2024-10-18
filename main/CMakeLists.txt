cmake_minimum_required(VERSION 3.13)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

project(main C CXX ASM)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/modules)

add_compile_options(
    -mcpu=cortex-m4
    -mthumb
    -D__SAME53N19A__
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    -Wall
)

# and the linker options
add_link_options(
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -larm_cortexM4lf_math
    -mthumb
)