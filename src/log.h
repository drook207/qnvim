// SPDX-FileCopyrightText: 2022 Mikhail Zolotukhin <mail@gikari.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QLoggingCategory>

/**
 * @file log.h
 * @brief Qt logging category declarations for the QNVim plugin.
 *
 * Use these categories with Qt's @c qDebug / @c qWarning / @c qInfo macros to
 * produce filterable log output.  Filtering can be controlled at runtime via
 * the @c QT_LOGGING_RULES environment variable or a @c qtlogging.ini file, e.g.:
 * @code
 * QT_LOGGING_RULES="qtcreator.plugin.qnvim.*=true"
 * @endcode
 */

/**
 * @var lcQNvimCore
 * @brief Logging category for general QNVim core events (default level: debug).
 *
 * Category name: @c qtcreator.plugin.qnvim.core
 */
Q_DECLARE_LOGGING_CATEGORY(lcQNvimCore);

/**
 * @var lcNVimBuffer
 * @brief Logging category for Neovim buffer synchronization events (default level: warning).
 *
 * Category name: @c qtcreator.plugin.qnvim.buffer
 */
Q_DECLARE_LOGGING_CATEGORY(lcNVimBuffer);
