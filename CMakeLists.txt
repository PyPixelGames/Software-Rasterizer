cmake_minimum_required(VERSION 3.20)

set(APP_NAME "SoRas")

project(${APP_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

add_executable(${APP_NAME}
    src/main.cpp
    src/types.hpp
    src/rendering.cpp
	src/helper.cpp
	src/loader.cpp
	src/stb_image.h
)

target_include_directories(SoRas PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${APP_NAME} PRIVATE SDL3::SDL3)
