cmake_minimum_required(VERSION 3.13)
project(BPKproject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBVERSION 7.3.1)
set(libpqxxdir "third-party/libpqxx-${LIBVERSION}")     # LIBVERSION set above
set(SKIP_BUILD_TEST on)
set(BUILD_SHARED_LIBS OFF)

set(PostgreSQL_FOUND true)
set(PostgresSQL_INCLUDE_DIR ${PostgresSQL_INCLUDE_DIRS})
set(PostgresSQL_TYPE_INCLUDE_DIR ${PostgresSQL_INCLUDE_DIRS})

add_subdirectory(${libpqxxdir})

include_directories(third-party/date third-party/libpqxx-7.3.1/include third-party/websocketpp-0.8.2 third-party/json-3.9.1/include)
add_subdirectory(model)
include_directories(model)
add_subdirectory(client)
add_subdirectory(server)