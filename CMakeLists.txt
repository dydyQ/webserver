cmake_minimum_required(VERSION 3.22)
project(sylarWebserver)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(/usr/local/include/yaml-cpp/)
include_directories(.)

set(LIB_SRC
         Log.cpp
        util.cpp
        config.cpp)

add_library(sylar SHARED ${LIB_SRC})

add_executable(test_config ./test/test_config.cpp)
add_dependencies(test_config sylar)
target_link_libraries(test_config sylar /usr/local/lib/libyaml-cpp.a)

add_executable(test_log ./test/test_log.cpp)
add_dependencies(test_log sylar)
target_link_libraries(test_log sylar)

