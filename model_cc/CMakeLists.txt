cmake_minimum_required(VERSION 3.13)

project(ClearCore C CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME} SHARED ${CPP_FILES})

target_sources(ClearCore PRIVATE
    Test.cpp
)

