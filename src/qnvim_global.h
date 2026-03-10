// SPDX-FileCopyrightText: 2018-2019 Sassan Haradji <sassanh@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QtGlobal>

/**
 * @def QNVIMSHARED_EXPORT
 * @brief Marks symbols for export or import depending on whether the library is being built.
 *
 * When @c QNVIM_LIBRARY is defined (i.e. building the plugin itself), this expands to
 * @c Q_DECL_EXPORT so symbols are visible to consumers.  Otherwise it expands to
 * @c Q_DECL_IMPORT.
 */
#if defined(QNVIM_LIBRARY)
#  define QNVIMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QNVIMSHARED_EXPORT Q_DECL_IMPORT
#endif
