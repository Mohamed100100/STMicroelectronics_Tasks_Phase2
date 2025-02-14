cmake_minimum_required(VERSION 3.5)

project(WireFish VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libpcap)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/protocols/DNSclass.cpp
    src/protocols/FTPclass.cpp
    src/protocols/HTTPclass.cpp
    src/protocols/ICMPclass.cpp
    src/protocols/IPclass.cpp
    src/protocols/TCPclass.cpp
    src/protocols/UDPclass.cpp
    src/sniffer/PacketSniffer.cpp
    src/PacketDetailsDialog.h
    src/SplashScreen.h
)

add_executable(WireFish
    ${PROJECT_SOURCES}
)

target_link_libraries(WireFish PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    pcap
)
