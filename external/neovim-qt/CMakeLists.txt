# SPDX-FileCopyrightText: 2022 Mikhail Zolotukhin <mail@gikari.com>
# SPDX-License-Identifier: MIT

include(FetchContent)

FetchContent_Declare(
  neovimqt
  GIT_REPOSITORY https://github.com/equalsraf/neovim-qt
  GIT_TAG v0.2.19
)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(WITH_QT "Qt6" CACHE STRING "Specify which Qt version to use with neovim-qt")

FetchContent_MakeAvailable(neovimqt)
