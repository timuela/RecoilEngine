cmake_minimum_required(VERSION 3.0)
project(flecs)
add_library(flecs flecs.c)
target_include_directories(flecs PRIVATE /)
target_compile_definitions(flecs PRIVATE
	FLECS_CUSTOM_BUILD
	FLECS_CPP
	FLECS_SYSTEM
	FLECS_JSON
	FLECS_LOG
)