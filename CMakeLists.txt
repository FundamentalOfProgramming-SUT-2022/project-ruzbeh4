cmake_minimum_required(VERSION 3.22)
project(project C)

set(CMAKE_C_STANDARD 99)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
include_directories(include)
file(GLOB SOURCES "*.c")
#set(CMAKE_CXX_FLAGS "-lncurses")
add_executable(project main.c test.c test2.c string.c tree.h root/test3.c ${SOURCES})

target_link_libraries(${PROJECT_NAME} -lncursesw)