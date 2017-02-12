cmake_minimum_required(VERSION 3.6)
project(AStarVisualization)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -Wall")

find_package(Qt5Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

qt5_wrap_ui(WIDGET_UI_MOC ${PROJECT_SOURCE_DIR}/mainwindow.ui)
qt5_wrap_ui(DIALOG_UI_MOC ${PROJECT_SOURCE_DIR}/dialog.ui)

add_executable("GreedySnake"
    main.cpp
    mainwindow.cpp
    dialog.cpp
    pathfinding.cpp
    square.cpp
    ${WIDGET_UI_MOC}
    ${DIALOG_UI_MOC}
    )

target_link_libraries("GreedySnake" Qt5::Widgets)
