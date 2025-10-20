cmake_minimum_required(VERSION 3.16)

project(kurs_project LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(app_icon_resource_window "${CMAKE_SOURCE_DIR}/resources/resource_icon.rc")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

add_executable(kurs_project
  ${CMAKE_SOURCE_DIR}/src/main.cpp
  ${CMAKE_SOURCE_DIR}/src/main.h
  ${CMAKE_SOURCE_DIR}/src/MainWindow.hpp
  ${CMAKE_SOURCE_DIR}/src/MainWindow.cpp
  ${CMAKE_SOURCE_DIR}/src/converter.h
  ${CMAKE_SOURCE_DIR}/src/converter.cpp
  ${CMAKE_SOURCE_DIR}/src/SessionsManager.hpp
  ${CMAKE_SOURCE_DIR}/src/SessionsManager.cpp
  ${CMAKE_SOURCE_DIR}/src/ServerRequester.hpp
  ${CMAKE_SOURCE_DIR}/src/ServerRequester.cpp

  resources/resources.qrc
  ${app_icon_resource_window}

)

find_library(AVFORMAT avformat PATHS "C:/msys64/ucrt64/lib")
find_library(AVCODEC avcodec PATHS "C:/msys64/ucrt64/lib")
find_library(AVUTIL avutil PATHS "C:/msys64/ucrt64/lib")
find_library(SWSCALE swscale PATHS "C:/msys64/ucrt64/lib")
find_library(SWRESAMPLE swresample PATHS "C:/msys64/ucrt64/lib")
find_library(BOOST_ASIO ws2_32 mswsock "C:/msys64/ucrt64/lib")

target_include_directories(kurs_project PRIVATE
  "C:\\msys64\\ucrt64\\include"
)
target_link_libraries(kurs_project
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Gui
  Qt${QT_VERSION_MAJOR}::Widgets
  ${AVFORMAT}#можно просто "C:/msys64/ucrt64/lib/libavformat.a"
  ${AVCODEC}
  ${AVUTIL}
  ${SWSCALE}
  ${SWRESAMPLE}
  ${BOOST_ASIO}
)

target_compile_options(kurs_project PRIVATE
  -Wall
  -pedantic
)

include(GNUInstallDirs)
install(TARGETS kurs_project
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
