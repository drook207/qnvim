// SPDX-FileCopyrightText: 2018-2019 Sassan Haradji <sassanh@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QWidget>

namespace TextEditor {
class TextEditorWidget;
}

namespace QNVim {
namespace Internal {

/**
 * @class NumbersColumn
 * @brief Overlay widget that renders Vim-style relative line numbers inside a Qt Creator editor.
 *
 * Qt Creator's built-in line-number gutter shows absolute line numbers.  When Neovim's
 * @c relativenumber option is active, QNVim overlays a @c NumbersColumn widget on top of
 * the editor's extra area to display relative line numbers (distance from the current line)
 * instead.  The current line itself displays @c 0 (or its absolute number when @c number
 * is also set in Neovim).
 *
 * The widget is transparent to mouse events and repositions itself automatically when the
 * editor is resized, scrolled, or its font settings change.
 */
class NumbersColumn : public QWidget {
    Q_OBJECT

  public:
    /// Constructs the column widget.  The widget has no parent initially; call setEditor()
    /// to attach it to a @c TextEditorWidget.
    NumbersColumn();

    /**
     * @brief Attaches (or detaches) the column to a text editor widget.
     *
     * Connects to the editor's scroll, cursor-change, and document-change signals so that
     * the column repaints and repositions itself as needed.  Pass @c nullptr to detach from
     * the current editor.
     *
     * @param editor  The editor widget to track, or @c nullptr to detach.
     */
    void setEditor(TextEditor::TextEditorWidget *editor);

    /**
     * @brief Enables or disables the absolute line number on the cursor line.
     *
     * When @p number is @c true the cursor line shows its absolute line number (mirroring
     * Neovim's combined @c number + @c relativenumber behaviour).  When @c false every
     * line (including the cursor line) shows its relative distance.
     *
     * @param number  @c true to show absolute number on cursor line.
     */
    void setNumber(bool number);

    /**
     * @brief Recalculates the widget geometry to stay aligned with the editor's extra area.
     *
     * Called automatically when the editor is resized, scrolled, or font settings change.
     * May also be called manually after layout changes.
     */
    void updateGeometry();

  protected:
    /**
     * @brief Renders the relative line numbers for all visible blocks.
     * @param event  The paint event describing the dirty region.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Watches the attached editor for resize and move events.
     *
     * Returns @c false so events are never consumed.
     *
     * @param object  The watched object (the editor widget).
     * @param event   The intercepted event.
     * @return Always @c false.
     */
    bool eventFilter(QObject *object, QEvent *event) override;

  private:
    /// Whether to render the absolute line number on the cursor line.
    bool mNumber = false;
    /// The editor widget this column is currently attached to.
    TextEditor::TextEditorWidget *mEditor = nullptr;
};

} // namespace Internal
} // namespace QNVim
