cmake_minimum_required(VERSION 3.5)

project(Ordering-System-Client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets Gui Core Network Sql REQUIRED)

qt5_add_resources(qrc_FILES res.qrc)

if(ANDROID)
  add_library(Ordering-System-Client SHARED
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
  )
else()
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
  )
endif()

target_link_libraries(Ordering-System-Client PRIVATE Qt5::Widgets Qt5::Network Qt5::Gui Qt5::Core Qt5::Sql)
