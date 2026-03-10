// SPDX-FileCopyrightText: 2018-2019 Sassan Haradji <sassanh@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file qnvimconstants.h
 * @brief Action and menu identifiers used throughout the QNVim plugin.
 */

namespace QNVim {

/**
 * @namespace QNVim::Constants
 * @brief Compile-time string constants for Qt Creator action/menu registration.
 */
namespace Constants {

/// Qt Creator action ID for toggling the QNVim plugin on and off.
const char TOGGLE_ID[] = "QNVim.Toggle";

/// Qt Creator menu container ID for the QNVim top-level menu entry.
const char MENU_ID[] = "QNVim.Menu";

} // namespace Constants
} // namespace QNVim
