<<<<<<< HEAD
set(CMAKE_CONFIGURATION_TYPES Debug;Release)

cmake_minimum_required(VERSION 3.13)

project(BPK VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(RESOURCE resources.qrc)
set(UI mainwindow.ui)
qt6_wrap_ui(UI_HEADERS ${UI})

set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/chat_view.cpp
        src/message_view.cpp

        include/mainwindow.h
        include/chat_view.h
        include/message_view.h
        )

add_executable(${PROJECT_NAME} ${UI} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${Qt6Widgets_LIBRARIES} Qt6::Core Qt6::Widgets Qt6::Gui)
set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
target_include_directories(${PROJECT_NAME} PUBLIC include)

set(QT_MINGW "C:/Qt/6.0.1/mingw81_64")

if(WIN32)
    if($<CONFIG:Debug>)
        get_target_property(WIDGETDLL Qt6::Widgets IMPORTED_LOCATION_DEBUG)
        get_target_property(COREDLL Qt6::Core IMPORTED_LOCATION_DEBUG)
        get_target_property(GUIDLL Qt6::Gui IMPORTED_LOCATION_DEBUG)
    else($<CONFIG:Debug>)
        get_target_property(WIDGETLL Qt6::Widgets IMPORTED_LOCATION_RELEASE)
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
=======
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
>>>>>>> added cmake for ui
