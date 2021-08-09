cmake_minimum_required(VERSION 3.19)
set(TARGET_NAME EclipseSecondDawnSimulator)

file(GLOB_RECURSE sources ./**.cpp ./**.hpp)
add_executable(${TARGET_NAME} ${sources})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} PRIVATE esd_core)