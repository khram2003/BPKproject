set(CMAKE_CONFIGURATION_TYPES Debug)

cmake_minimum_required(VERSION 3.13)

project(BPK VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(RESOURCE resources.qrc)
set(UI mainwindow.ui auth.ui)
qt6_wrap_ui(UI_HEADERS ${UI})

set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/chat_view.cpp
        src/message_view.cpp
        src/auth.cpp
        src/socket.cpp
        src/user.cpp

        include/mainwindow.h
        include/chat_view.h
        include/message_view.h
        include/auth.h
        include/socket.h
        include/user.h
        )

set(NETWORKING_LIBS)
if (WIN32 AND NOT MSVC AND NOT BORLAND)
    set(NETWORKING_LIBS "ws2_32")
endif ()

add_executable(${PROJECT_NAME} ${UI} ${SOURCES})
set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} ${Qt6Widgets_LIBRARIES} Qt6::Core Qt6::Widgets Qt6::Gui ${NETWORKING_LIBS} ${Boost_LIBRARIES} pthread)

set(QT_MINGW "C:/Qt/6.1.0/mingw81_64")

if(WIN32)
    if($<CONFIG:Debug>)
        get_target_property(WIDGETDLL Qt6::Widgets IMPORTED_LOCATION_DEBUG)
        get_target_property(COREDLL Qt6::Core IMPORTED_LOCATION_DEBUG)
        get_target_property(GUIDLL Qt6::Gui IMPORTED_LOCATION_DEBUG)
    else($<CONFIG:Debug>)
        get_target_property(WIDGETDLL Qt6::Widgets IMPORTED_LOCATION_RELEASE)
        get_target_property(COREDLL Qt6::Core IMPORTED_LOCATION_RELEASE)
        get_target_property(GUIDLL Qt6::Gui IMPORTED_LOCATION_RELEASE)
    endif($<CONFIG:Debug>)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Widgets>
            $<TARGET_FILE:Qt6::Core>
            $<TARGET_FILE:Qt6::Gui>
            ${QT_MINGW}/bin/libgcc_s_seh-1.dll
            ${QT_MINGW}/bin/libstdc++-6.dll
            ${QT_MINGW}/bin/libwinpthread-1.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif(WIN32)