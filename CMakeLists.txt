#设置cmake版本号
cmake_minimum_required(VERSION 2.8.11)

#设置工程名称
PROJECT(QT_CALCULATOR)

#find QT
find_package(Qt5Widgets)


#新添加
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#path definition
set(PROJECT_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "MAIN directory")
set(PROJECT_ALGORITHM_DIR "${PROJECT_MAIN_DIR}/algorithm" CACHE PATH "ALGORITHM directory")
set(PROJECT_UI_DIR "${PROJECT_MAIN_DIR}/ui" CACHE PATH "UI directory")

#header file
file(GLOB project_algorithm_header_files ${PROJECT_ALGORITHM_DIR}/*.h)
file(GLOB project_ui_header_files ${PROJECT_UI_DIR}/*.h)

#source file
file(GLOB project_main_source_files ${PROJECT_MAIN_DIR}/*.cpp)
file(GLOB project_algorithm_source_files ${PROJECT_ALGORITHM_DIR}/*.cpp)
file(GLOB project_ui_source_files ${PROJECT_UI_DIR}/*.cpp)

#group
source_group("Header Files\\algorithm" FILES ${project_algorithm_header_files})
source_group("Header Files\\ui" FILES ${project_ui_header_files})
source_group("Source Files" FILES ${project_main_source_files})
source_group("Source Files\\algorithm" FILES ${project_algorithm_source_files})
source_group("Source Files\\ui" FILES ${project_ui_source_files})

QT5_WRAP_CPP(project_ui_moc_source_files ${project_ui_header_files})
source_group("Generate Files" FILES ${project_ui_moc_source_files})

# Add the include directories for VTK and Qt 5 Widgets module to the compile lines.
include_directories(${PROJECT_MAIN_DIR}
					${PROJECT_ALGORITHM_DIR}
					${PROJECT_UI_DIR}
					${Qt5Widgets_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
			   ${project_algorithm_header_files}
			   ${project_ui_header_files}
			   ${project_main_source_files}
			   ${project_algorithm_source_files}
			   ${project_ui_source_files}
			   ${project_ui_moc_source_files})

#设定可执行二进制文件的目录
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin) 

#lib
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
