cmake_minimum_required(VERSION 3.21)
project(FooTerm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF (${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_definitions(-DDEBUG=1)
ENDIF ()

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(VTE REQUIRED vte-2.91)

link_directories(
        ${GTKMM_LIBRARY_DIRS})

include_directories(
        ${GTKMM_INCLUDE_DIRS})

include_directories(
        includes
)

add_executable(FooTerm src/main.cpp src/term_windows.cpp src/vte.cpp src/ssh.cpp src/eventloop.cpp src/panel.cpp includes/panel.hpp)

target_link_libraries(FooTerm
        ${GTKMM_LIBRARIES})

target_link_libraries(FooTerm ${VTE_LIBRARIES})
add_definitions(${VTE_CFLAGS} ${VTE_CFLAGS_OTHER})

add_subdirectory(third-party)
find_package(fmt)

target_link_libraries(FooTerm fmt::fmt ssh2 pthread util)