cmake_minimum_required(VERSION 3.16)

project(bancosangre VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Sql
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

qt_standard_project_setup()

qt_add_executable(bancosangre
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(bancosangre PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(MSVC)
    target_compile_options(bancosangre PRIVATE /W4)
else()
    target_compile_options(bancosangre PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

target_link_libraries(bancosangre PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)