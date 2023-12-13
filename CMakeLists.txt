cmake_minimum_required(VERSION 3.10)

project(CUBE LANGUAGES CXX)

# Strict CXX 11 Standard
#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)
