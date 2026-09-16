cmake_minimum_required(VERSION 3.20)

project(My_sniffer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(My_sniffer
    src/main.cpp
    src/fcn_helper.cpp
    src/nic/nic_linux.cpp
    src/nic/nic_windows.cpp
)

target_include_directories(My_sniffer
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(My_sniffer PRIVATE /W4)
else()
    target_compile_options(My_sniffer PRIVATE -Wall -Wextra -Wpedantic)
endif()