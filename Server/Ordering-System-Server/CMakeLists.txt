cmake_minimum_required(VERSION 3.5)

project(Ordering-System-Server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(Qt5 COMPONENTS Widgets Gui Core Network Sql Charts Multimedia REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR}/3rd)

qt5_add_resources(qrc_FILES res.qrc)

find_library(qhttpserver_lib NAMES qhttpserverd PATHS ${CMAKE_CURRENT_LIST_DIR}/3rd/qhttpserver/lib NO_DEFAULT_PATH)

add_executable(Ordering-System-Server
        ${qrc_FILES}
        main.cpp
        servermainwindow.cpp
        servermainwindow.h
        servermainwindow.ui
        dialogaddrecord.h
        dialogaddrecord.cpp
        dialogeditrecord.h
        dialogeditrecord.cpp
        convertpic.h
        convertpic.cpp
        dialoghistoryviewer.h
        dialoghistoryviewer.cpp
        dialogordersviewer.h
        dialogordersviewer.cpp
        Dish.h
        Dish.cpp
        timestamp.h
        timestamp.cpp
        httpfileload.h
        httpfileload.cpp
        httpfileupdate.h
        httpfileupdate.cpp
        mydatetimeselect.h
        mydatetimeselect.cpp
        mybutton.h
        mybutton.cpp
        dialogchartview.h
        dialogchartview.cpp
        3rd/json.hpp
        SimpleWebServer.cpp SimpleWebServer.h TcpServer.cpp TcpServer.h)

target_link_libraries(Ordering-System-Server PRIVATE Qt5::Widgets Qt5::Network Qt5::Gui Qt5::Core Qt5::Sql Qt5::Charts Qt5::Multimedia ${qhttpserver_lib})
