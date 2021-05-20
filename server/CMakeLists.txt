set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")

add_executable(server server.cpp)
target_link_libraries(server model)
