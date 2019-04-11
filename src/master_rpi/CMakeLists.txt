cmake_minimum_required(VERSION 3.0)

project(mesh_master)

find_package(Boost REQUIRED COMPONENTS system thread log program_options chrono)
find_package(Threads REQUIRED)

find_package(OpenSSL 1.0.1 REQUIRED)
set(OPENSSL_VERSION "1.0.1")

# Configure compiler options ...
message("-- compiler: ${CMAKE_CXX_COMPILER_ID}")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    message("-- configuring clang options")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch x86_64 -std=c++11 -stdlib=libc++ -DBOOST_LOG_DYN_LINK -Wno-deprecated-declarations")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    message("-- configuring gcc options")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -DBOOST_LOG_DYN_LINK -lrf24-bcm -lrf24network -lrf24mesh ")

endif ()

# Project construction ...

# source files ...
add_executable(${PROJECT_NAME} source/main.cpp
        source/includes/func.cpp
        source/includes/radio.cpp
        source/includes/payloads.cpp
        source/includes/microsvc_controller.cpp
        source/includes/foundation/network_utils.cpp
        source/includes/foundation/basic_controller.cpp)

# headers search paths ...
set(CPPRESTSDK_INCLUDE_DIR "./libs/cpprestsdk/Release/include")
set(MICROSERVICE_INCLUDE_DIR "source/includes/foundation/include")
message(MICROSERVICE " ${MICROSERVICE_INCLUDE_DIR}")
set(SOURCE_INCLUDES "./source/includes")

set(HEADER_SEARCH_PATHS ${CPPRESTSDK_INCLUDE_DIR} ${MICROSERVICE_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${SOURCE_INCLUDES})

# library search paths ...
set(OPENSSL_LIBS "${OPENSSL_LIBRARIES}")
set(CPPRESTSDK_LIBRARY "${PROJECT_SOURCE_DIR}/libs/cpprestsdk/build.release/Binaries/libcpprest.a")

set(LIBRARIES_SEARCH_PATHS ${CPPRESTSDK_LIBRARY} ${OPENSSL_LIBS} ${Boost_LIBRARIES})

message(BOOST_LIBS " ${Boost_LIBRARIES}")
message(OPENSSL_LIBS " ${OPENSSL_LIBRARIES}")
message(CPPRESTSDK_LIBRARY " ${CPPRESTSDK_LIBRARY}")
message(LIBRARIES_SEARCH_PATHS " ${LIBRARIES_SEARCH_PATHS}")
message(SOURCE_INCLUDES " ${SOURCE_INCLUDES}")

include_directories(${HEADER_SEARCH_PATHS})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES_SEARCH_PATHS})
target_link_libraries(${PROJECT_NAME} ${RF24_LIB})


