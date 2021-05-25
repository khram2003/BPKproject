set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")

add_library(model STATIC model.cpp)
target_link_libraries(model pqxx)