find_package(Boost 1.67.0 REQUIRED COMPONENTS system) 
include_directories(${Boost_INCLUDE_DIRS})

add_library(poco_simple_handler SimplePocoHandler.cpp SimplePocoHandler.h)
add_library(Req Req.cpp)
target_link_libraries(poco_simple_handler amqpcpp PocoNet PocoFoundation)
target_link_libraries(Req ${Boost_LIBRARIES})


    add_executable(zayac "zayac.cpp")
    target_link_libraries(zayac ${Boost_LIBRARIES} Req amqpcpp poco_simple_handler nlohmann_json::nlohmann_json)    

