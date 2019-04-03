cmake_minimum_required(VERSION 3.0)

project(rf24)


set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_FLAGS "-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -std=c++0x -lrf24-bcm -lrf24network -lrf24mesh ")

# source files ...
add_executable(${PROJECT_NAME} ./source/master_rpi.cpp)

set(SOURCE_INCLUDES "./source/includes_rpi")

set(HEADER_SEARCH_PATHS ${SOURCE_INCLUDES})

message(SourceIncludes " ${SOURCE_INCLUDES}")

include_directories(${HEADER_SEARCH_PATHS})

target_link_libraries(${PROJECT_NAME} ${RF24_LIB})
