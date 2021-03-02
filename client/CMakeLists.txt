cmake_minimum_required(VERSION 3.16.0)

list(APPEND CMAKE_PREFIX_PATH "C:\\Qt\\6.0.1\\mingw81_64")

project(BPK VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

qt_wrap_ui (mainwindow  mainwindow.ui)
add_executable(BPK
        mainwindow.ui
        mainwindow.cpp
        main.cpp
        )
target_link_libraries(BPK PRIVATE Qt6::Widgets)