cmake_minimum_required(VERSION 3.5)

project(remote LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 


add_subdirectory(libela)
add_subdirectory(librudp)
add_subdirectory(foils_hid)
add_subdirectory(libelaqt)
add_subdirectory(remote)
add_subdirectory(remote-cli)
