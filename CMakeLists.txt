cmake_minimum_required(VERSION 3.5)
project(SoftwareProgrammierung_1)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES src/assigment.c src/helper.c src/helper.h src/hashmap.c src/hashmap.h)
add_executable(assignement ${SOURCE_FILES})