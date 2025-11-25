cmake_minimum_required(VERSION 3.20)
project(EclipseSecondDawnSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)

add_executable(eclipse_sim
    src/main.cpp
    src/render/bitmap_font.cpp
    src/game/tech_catalog.cpp
    src/game/battle_simulator.cpp
)

target_include_directories(eclipse_sim PRIVATE include)

target_link_libraries(eclipse_sim PRIVATE SDL2::SDL2)

add_executable(battle_sim_tests
    tests/battle_simulator_spec.cpp
    src/game/tech_catalog.cpp
    src/game/battle_simulator.cpp
)

target_include_directories(battle_sim_tests PRIVATE include)
