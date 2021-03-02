cmake_minimum_required(VERSION 3.13)
project(BPKproject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL)

include_directories(third-party/date third-party/libpqxx-7.3.1/include third-party/websocketpp-0.8.2/websocketpp)
add_subdirectory(third-party/libpqxx-7.3.1)
add_subdirectory(model)
include_directories(model)
add_subdirectory(client)
add_subdirectory(server)