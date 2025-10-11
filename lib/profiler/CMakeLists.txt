cmake_minimum_required(VERSION 3.7)

project(profiler LANGUAGES C VERSION 0.1.0)

add_library(profiler
	src/rdtsc.c
	src/profiler.c
	src/repetition_tester.c
	src/perf.c
	)

target_include_directories(profiler
	PUBLIC include
	PRIVATE include/internal
	)
