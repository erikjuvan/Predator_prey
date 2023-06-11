cmake_minimum_required(VERSION 3.15.5)
project(predator_prey)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo") 

find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 17)

target_sources(${PROJECT_NAME} PRIVATE 
	src/main.c
	)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
