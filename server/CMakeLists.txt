set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pthread")

add_executable(server server.cpp)
target_link_libraries(server model pqxx pq)