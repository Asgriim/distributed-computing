cmake_minimum_required(VERSION 3.27)
project(distt_sys C)

set(CMAKE_C_STANDARD 17)


#execute_process(COMMAND bash -c "tar -czf pa1.tar.gz ../pa1" )

# Create a custom target to run the command
add_custom_target(create_tar ALL
        COMMAND bash -c "tar -czf pa1.tar.gz pa1"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Creating tarball"
)

add_compile_options(-Wall -pedantic)

file(GLOB SRC_FILES pa1/*.c)

add_executable(lab1 ${SRC_FILES})

# Make the executable depend on the custom target
add_dependencies(lab1 create_tar)