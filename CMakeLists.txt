# SPDX-FileCopyrightText: 2022 Mikhail Zolotukhin <mail@gikari.com>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.22)

project(QNVim)

# Remove when sharing with others.
list(APPEND CMAKE_PREFIX_PATH "/home/drook207/Qt/Tools/QtCreator/lib/cmake/QtCreator")


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QtCreator REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

add_subdirectory(external)


if (MSVC)
  add_compile_options("/permissive-")
endif()


add_subdirectory(src)
