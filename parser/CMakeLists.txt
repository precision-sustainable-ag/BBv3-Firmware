cmake_minimum_required(VERSION 3.13)
project(parser CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(parser)

target_sources(parser PRIVATE
    parser.cpp
)

target_link_libraries(parser ClearCore)
