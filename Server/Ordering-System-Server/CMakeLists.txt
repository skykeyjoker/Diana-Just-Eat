cmake_minimum_required(VERSION 3.5)

project(Ordering-System-Server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(Qt5 COMPONENTS Widgets Gui Core Network Sql Charts Multimedia REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR}/3rd)

qt5_add_resources(qrc_FILES res.qrc)

add_executable(Ordering-System-Server
        ${qrc_FILES}
        main.cpp
        servermainwindow.cpp
        servermainwindow.h
        servermainwindow.ui
        EncryptDecrypt.h
        EncryptDecrypt.cpp
        dialogaddrecord.h
        dialogaddrecord.cpp
        dialogeditrecord.h
        dialogeditrecord.cpp
        convertpic.h
        convertpic.cpp
        writejson.h
        dialoghistoryviewer.h
        dialoghistoryviewer.cpp
        dialogordersviewer.h
        dialogordersviewer.cpp
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
        )

target_link_libraries(Ordering-System-Server PRIVATE Qt5::Widgets Qt5::Network Qt5::Gui Qt5::Core Qt5::Sql Qt5::Charts Qt5::Multimedia)
