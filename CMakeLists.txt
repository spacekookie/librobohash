# ////////////////////////////////
# //
# // This file is part of librobohash
# //    and thus free software under the
# //    GNU Public License 2.0
# //
# ///////////


cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(CMAKE_BUILD_TYPE Debug)

project(robohash)

#find_package (PkgConfig)
#pkg_check_modules (EM REQUIRED mbedtls)

# Define our library in cmake
add_library(robohash
        robohash/robohash.c
        robohash/mbedtls/sha512.c)
set_target_properties(robohash PROPERTIES PREFIX "lib")

#target_link_libraries(robohash ${EM_LIBRARIES})
#target_include_directories(robohash ${EM_INCLUDES})

target_include_directories(robohash PUBLIC "robohash")


# Then make a testing binary for us to use
add_executable(robohash-test robohash/tester.c)
target_link_libraries(robohash-test robohash)

