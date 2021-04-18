cmake_minimum_required(VERSION 3.13)

project(BPK VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTOUIC OFF)

set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXE_LINKER_FLAGS "-static")

#find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)

add_definitions(${Qt6Widgets_DEFINITIONS})
include_directories(${Qt6Widgets_INCLUDES})

include_directories(${CMAKE_SOURCE_DIR}/include)

# Use Qt5 ressources
set(RESOURCE resources.qrc)
set(UI mainwindow.ui)
qt6_wrap_ui(UI_HEADERS ${UI})

set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/chat_view.cpp
        src/message_view.cpp
        src/client.cpp

        include/mainwindow.h
        include/chat_view.h
        include/message_view.h
        )


add_executable(${PROJECT_NAME} ${UI} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${Qt6Widgets_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Qt6::Core)
target_link_libraries(${PROJECT_NAME} Qt6::Widgets)