cmake_minimum_required(VERSION 3.6)
project(CGI_P1)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})

add_executable(CGI_P1 ${SOURCE_FILES})

target_link_libraries(CGI_P1 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
