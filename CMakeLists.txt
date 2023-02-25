set(PROJECT zayac)
project(${PROJECT} C CXX)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wextra -pedantic -pthread -lboost_system")
set(JSON_Install OFF CACHE INTERNAL "")
include_directories(3rdparty/json-develop
		    3rdparty/amqp-cpp
                    3rdparty/
)
add_subdirectory(3rdparty/json-develop)
add_subdirectory(3rdparty/amqp-cpp)
add_subdirectory(src)
