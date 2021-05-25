cmake_minimum_required(VERSION 3.12)
project(BPKproject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_CLIENT "Build client" ON)

find_package(PostgreSQL)

set(LIBPQXX_VERSION "libpqxx-7.3.1")
set(WEBSOCKETPP_VERSION "websocketpp-0.8.2")
set(JSON_VERSION "json-3.9.1")

include_directories(third-party/date third-party/${LIBPQXX_VERSION}/include third-party/${WEBSOCKETPP_VERSION} third-party/${JSON_VERSION}/include)
add_subdirectory(third-party/${LIBPQXX_VERSION})
add_subdirectory(third-party/${WEBSOCKETPP_VERSION})
add_subdirectory(model)
include_directories(model)
if (BUILD_CLIENT)
    add_subdirectory(client)
endif()
add_subdirectory(server)