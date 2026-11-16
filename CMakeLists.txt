cmake_minimum_required(VERSION 3.21)
project(DicomViewer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(DCMTK CONFIG REQUIRED)
add_executable(DicomViewer
    src/main.cpp
    src/main_window.cpp
    src/image_preview.cpp
)
target_link_libraries(DicomViewer PRIVATE Qt6::Widgets DCMTK::dcmimgle DCMTK::dcmjpeg)
message(STATUS "Qt version: ${Qt6_VERSION}")
