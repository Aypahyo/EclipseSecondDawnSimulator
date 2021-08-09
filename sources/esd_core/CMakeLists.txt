cmake_minimum_required(VERSION 3.19)
set(TARGET_NAME esd_core)

file(GLOB_RECURSE sources ./**.cpp ./**.hpp)
add_library(${TARGET_NAME} STATIC ${sources})
target_include_directories(${TARGET_NAME} PUBLIC ..)