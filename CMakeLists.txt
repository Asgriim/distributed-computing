cmake_minimum_required(VERSION 3.27)
project(distt_sys C)

set(CMAKE_C_STANDARD 17)


execute_process(COMMAND bash -c "tar -czf pa1.tar.gz ./pa1")

add_compile_options(-Wall -pedantic)

add_executable(distt_sys pa1/main.c)

