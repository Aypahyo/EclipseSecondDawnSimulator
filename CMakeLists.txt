cmake_minimum_required(VERSION 3.20)

set(project_version 0.0.0)
set(project_description "EclipseSecondDawnSimulator")
set(project_homepage "https://github.com/Aypahyo/EclipseSecondDawnSimulator")

project(FlayedStar
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C CXX
        )

set(CMAKE_CXX_STANDARD 20)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(sources/EclipseSecondDawnSimulator)
add_subdirectory(sources/esd_core)
add_subdirectory(sources/esd_test)


