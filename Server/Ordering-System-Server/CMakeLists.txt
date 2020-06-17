cmake_minimum_required(VERSION 3.5)

project(Ordering-System-Server LANGUAGES CXX)

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

find_package(Qt5 COMPONENTS Widgets Gui Core Network Sql Charts Multimedia REQUIRED)
qt5_add_resources(qrc_FILES res.qrc)

if(ANDROID)
  add_library(Ordering-System-Server SHARED
    ${qrc_FILES}
    EncryptDecrypt.h
    EncryptDecrypt.cpp
    main.cpp
    servermainwindow.cpp
    servermainwindow.h
    servermainwindow.ui
    readjson.h
    readjson.cpp
    dialogaddrecord.h
    dialogaddrecord.cpp
    dialogeditrecord.h
    dialogeditrecord.cpp
    convertpic.h
    convertpic.cpp
    writejson.h
    writejson.cpp
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
  )
else()
  add_executable(Ordering-System-Server
    ${qrc_FILES}
    main.cpp
    servermainwindow.cpp
    servermainwindow.h
    servermainwindow.ui
    EncryptDecrypt.h
    EncryptDecrypt.cpp
    readjson.h
    readjson.cpp
    dialogaddrecord.h
    dialogaddrecord.cpp
    dialogeditrecord.h
    dialogeditrecord.cpp
    convertpic.h
    convertpic.cpp
    writejson.h
    writejson.cpp
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
  )
endif()

target_link_libraries(Ordering-System-Server PRIVATE Qt5::Widgets Qt5::Network Qt5::Gui Qt5::Core Qt5::Sql Qt5::Charts Qt5::Multimedia)
