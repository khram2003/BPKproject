cmake_minimum_required(VERSION 3.13)
project(BPKproject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")

add_subdirectory(model)
include_directories(model)
add_subdirectory(client)
add_subdirectory(server)