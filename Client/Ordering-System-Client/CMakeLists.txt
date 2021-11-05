cmake_minimum_required(VERSION 3.5)

project(Ordering-System-Client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(Qt5 COMPONENTS Widgets Gui Core Network Sql REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR}/3rd)

qt5_add_resources(qrc_FILES res.qrc)

add_executable(Ordering-System-Client
        ${qrc_FILES}
        main.cpp
        clientmainwindow.cpp
        clientmainwindow.h
        clientmainwindow.ui
        tcpclient.h
        tcpclient.cpp
        readjson.h
        readjson.cpp
        EncryptDecrypt.h
        EncryptDecrypt.cpp
        writejson.h
        writejson.cpp
        dialogsettings.h
        dialogsettings.cpp
        httpfiledownload.h
        httpfiledownload.cpp
        cartitem.h
        cartitem.cpp
        timestamp.h
        timestamp.cpp
        dialogcartview.h
        dialogcartview.cpp
        mybutton.h
        mybutton.cpp
        dialogcheckout.h
        dialogcheckout.cpp
        myspinbox.h
        myspinbox.cpp
        myspinboxdelegate.h
        myspinboxdelegate.cpp
        3rd/json.hpp
        Dish.cpp Dish.h)

target_link_libraries(Ordering-System-Client PRIVATE Qt5::Widgets Qt5::Network Qt5::Gui Qt5::Core Qt5::Sql)
