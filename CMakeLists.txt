cmake_minimum_required(VERSION 3.12)
project(BPKproject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_CLIENT "Build client" ON)

find_package(PostgreSQL)

include_directories(third-party/date third-party/libpqxx-7.3.1/include third-party/websocketpp-0.8.2 third-party/json-3.9.1/include)
add_subdirectory(third-party/libpqxx-7.3.1)
add_subdirectory(third-party/websocketpp-0.8.2)
add_subdirectory(model)
include_directories(model)
if (BUILD_CLIENT)
  add_subdirectory(client)
endif()
add_subdirectory(server)
