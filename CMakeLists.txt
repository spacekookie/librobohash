# ////////////////////////////////
# //
# // This file is part of librobohash
# //    and thus free software under the
# //    GNU Public License 2.0
# //
# ///////////


cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(CMAKE_BUILD_TYPE Debug)

project(robohash)

set(ROBO_SRC 
    # Embedded crypto-library
    lib/mbedtls/sha512.c
    # lib/mbedtls/sha256.c

    # Core robohash files
    lib/base64.c
        lib/rh.c
    lib/imgur.c)

# Define the librobohash library
add_library(librobohash ${ROBO_SRC})
target_include_directories(librobohash PUBLIC "include")
target_include_directories(librobohash PRIVATE "lib")
target_link_libraries(librobohash png)

# Then make a testing binary for us to use
add_executable(robotest tester.c)
target_link_libraries(robotest librobohash)

