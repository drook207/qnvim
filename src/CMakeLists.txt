# SPDX-FileCopyrightText: 2022 Mikhail Zolotukhin <mail@gikari.com>
# SPDX-License-Identifier: MIT

add_qtc_plugin(QNVim
  PLUGIN_DEPENDS
    QtCreator::Core
    QtCreator::TextEditor
    QtCreator::ProjectExplorer
  DEPENDS
    Qt::Widgets
    QtCreator::ExtensionSystem
    QtCreator::Utils
    neovim-qt
    neovim-qt-gui
  SOURCES
    log.cpp
    log.h
    numbers_column.cpp
    numbers_column.h
    qnvim_global.h
    qnvimconstants.h
    qnvimplugin.cpp
    qnvimplugin.h
    qnvimcore.cpp
    qnvimcore.h
)
# Enable the Run button in Qt Creator
get_target_property(QtCreatorCorePath QtCreator::Core LOCATION)
message(STATUS "QtCreator Path: ${QtCreatorCorePath}")
find_program(QtCreatorExecutable
  NAMES
    qtcreator "Qt Creator"
  #PATHS
   # "${QtCreatorCorePath}/../../../bin"
    #"${QtCreatorCorePath}/../../../MacOS"
  #NO_DEFAULT_PATH
)
if (QtCreatorExecutable)
  message(STATUS "Add qtcreator custom target")
  add_custom_target(RunQtCreator
    COMMAND ${QtCreatorExecutable} -pluginpath $<TARGET_FILE_DIR:QNVim>
    DEPENDS QNVim
  )
  set_target_properties(RunQtCreator PROPERTIES FOLDER "qtc_runnable")
endif()
