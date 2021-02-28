set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")

add_executable(client client.cpp)
target_link_libraries(client model)