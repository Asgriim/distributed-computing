cmake_minimum_required(VERSION 3.27)
project(distt_sys C)

set(CMAKE_C_STANDARD 99)

set(LAB_NUM 3)
set(TARGET lab${LAB_NUM})
string(REPLACE "%" ${LAB_NUM} TAR_COMMAND "tar -czf pa%.tar.gz pa%" )

message(${TAR_COMMAND})
# Create a custom target to run the command
add_custom_target(create_tar ALL
        COMMAND bash -c "${TAR_COMMAND}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Creating tarball"
)

add_compile_options(-Wall -pedantic -std=c99 )

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/pa${LAB_NUM}/lib64)

file(GLOB SRC_FILES pa${LAB_NUM}/*.c)

add_executable(${TARGET} ${SRC_FILES})

# Make the executable depend on the custom target
add_dependencies(${TARGET} create_tar)

target_link_libraries(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pa${LAB_NUM}/lib64/libruntime.so)

