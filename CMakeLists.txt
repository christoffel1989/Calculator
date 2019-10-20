#cmake version
cmake_minimum_required(VERSION 2.8.11)

#project name
PROJECT(QT_CALCULATOR)

#find QT
find_package(Qt5Widgets)

#include current path
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#main
set(PROJECT_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "MAIN directory")
file(GLOB project_main_source_files ${PROJECT_MAIN_DIR}/*.cpp)
source_group("Source Files" FILES ${project_main_source_files})
#algorithm
set(PROJECT_ALGORITHM_DIR "${PROJECT_MAIN_DIR}/algorithm" CACHE PATH "ALGORITHM directory")
file(GLOB project_algorithm_header_files ${PROJECT_ALGORITHM_DIR}/*.h)
file(GLOB project_algorithm_source_files ${PROJECT_ALGORITHM_DIR}/*.cpp)
source_group("Header Files\\algorithm" FILES ${project_algorithm_header_files})
source_group("Source Files\\algorithm" FILES ${project_algorithm_source_files})
#ui
set(PROJECT_UI_DIR "${PROJECT_MAIN_DIR}/ui" CACHE PATH "UI directory")
file(GLOB project_ui_header_files ${PROJECT_UI_DIR}/*.h)
file(GLOB project_ui_source_files ${PROJECT_UI_DIR}/*.cpp)
source_group("Header Files\\ui" FILES ${project_ui_header_files})
source_group("Source Files\\ui" FILES ${project_ui_source_files})

# Add the include directories
include_directories(${PROJECT_MAIN_DIR}
					${PROJECT_ALGORITHM_DIR}
					${PROJECT_UI_DIR}
					${Qt5Widgets_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
			   ${project_algorithm_header_files}
			   ${project_ui_header_files}
			   ${project_main_source_files}
			   ${project_algorithm_source_files}
			   ${project_ui_source_files})

#lib
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

#multi-core compile and use iso c++17
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/MP /std:c++17")

#bin
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin) 