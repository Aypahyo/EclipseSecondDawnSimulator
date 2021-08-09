cmake_minimum_required(VERSION 3.19)
set(TARGET_NAME esd_core_test)

file(GLOB_RECURSE sources ./**.cpp ./**.hpp)
add_executable(${TARGET_NAME} ${sources})
target_link_libraries(${TARGET_NAME} PRIVATE CONAN_PKG::gtest)
target_link_libraries(${TARGET_NAME} PRIVATE esd_core)
