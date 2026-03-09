# SPDX-FileCopyrightText: 2022 Mikhail Zolotukhin <mail@gikari.com>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.22)

project(QNVim)

option(FETCH_QTC "Download Qt Creator development files automatically" ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_subdirectory(external)
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
  add_compile_options("/permissive-")
endif()


find_package(QtCreator REQUIRED COMPONENTS Core)
add_subdirectory(src)
