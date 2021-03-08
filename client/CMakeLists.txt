
find_package(Boost REQUIRED COMPONENTS thread)

message(${Boost_LIBRARIES} "Boost")

include_directories(third-party/date third-party/libpqxx-7.3.1/include third-party/websocketpp-0.8.2)

add_executable(client client.cpp)
target_include_directories(client PUBLIC "${CMAKE_SOURCE_DIR}/third_party/websocketpp-0.8.2")
target_link_libraries(client ${Boost_LIBRARIES} pthread)
target_link_libraries(client model)