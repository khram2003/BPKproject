set(CMAKE_CONFIGURATION_TYPES Debug;Release)

cmake_minimum_required(VERSION 3.13)

project(XINU VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
link_libraries(${CURL_LIBRARIES})

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui REQUIRED)
find_package(Qt6NetworkAuth REQUIRED)
find_package(Boost COMPONENTS thread system REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(RESOURCE resources.qrc)
set(UI mainwindow.ui auth.ui add_chat.ui trello_auth.ui add_member.ui view_boards.ui)
qt6_wrap_ui(UI_HEADERS ${UI})

set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/chat_view.cpp
        src/message_view.cpp
        src/auth.cpp
        src/socket.cpp
        src/user.cpp
        src/add_chat.cpp
        src/trello.cpp
        src/trello_auth.cpp
        src/popup.cpp
        src/auth_google.cpp
        src/add_member.cpp
        src/curl_raii.cpp
        src/clickablelabel.cpp


        include/mainwindow.h
        include/chat_view.h
        include/message_view.h
        include/auth.h
        include/socket.h
        include/user.h
        include/add_chat.h
        include/trello.h
        include/trello_auth.h
        include/popup.h
        include/auth_google.h
        include/add_member.h
        include/curl_raii.h
        include/clickablelabel.h src/view_boards.cpp include/view_boards.h)


set(NETWORKING_LIBS)
if (WIN32 AND NOT MSVC AND NOT BORLAND)
    set(NETWORKING_LIBS "ws2_32")
endif ()

add_executable(${PROJECT_NAME} ${UI} ${SOURCES})
set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
#PUBLIC linkage due to Qt bug: https://bugreports.qt.io/browse/QTBUG-93671
target_link_libraries(${PROJECT_NAME} PUBLIC ${Qt6Widgets_LIBRARIES} Qt6::Core Qt6::Widgets Qt6::Gui Qt6::NetworkAuth ${NETWORKING_LIBS} ${Boost_LIBRARIES})
