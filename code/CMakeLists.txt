cmake_minimum_required(VERSION 3.20)
project(caspercalendar CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CopperSpice REQUIRED)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/eventeditor.cpp
    src/preferences.cpp
    src/sanitizer.cpp
    src/dictionary.cpp
    src/diphone.cpp
    src/synthesizer.cpp
)

add_executable(caspercalendar ${SOURCES})

target_link_libraries(caspercalendar PRIVATE
    CopperSpice::CsCore
    CopperSpice::CsGui
    CopperSpice::CsSql
)
