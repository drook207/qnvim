// SPDX-FileCopyrightText: 2018-2019 Sassan Haradji <sassanh@gmail.com>
// SPDX-FileCopyrightText: 2023 Mikhail Zolotukhin <mail@gikari.com>
// SPDX-License-Identifier: MIT
#pragma once

#include <QColor>
#include <QMap>
#include <QObject>
#include <QPoint>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

namespace Core {
class IEditor;
}

namespace ProjectExplorer {
class Project;
}

namespace NeovimQt {
class NeovimConnector;
}

namespace QNVim {
namespace Internal {

class NumbersColumn;

/**
 * @class QNVimCore
 * @brief Core runtime of the QNVim plugin — owns the Neovim process and keeps Qt Creator
 *        editors in bidirectional sync with Neovim buffers.
 *
 * @c QNVimCore is created when QNVim is enabled and destroyed when it is disabled or Qt
 * Creator shuts down.  Its lifetime is managed by @c QNVimPlugin.
 *
 * ### Responsibilities
 * - Spawning the embedded Neovim process via @c NeovimQt::NeovimConnector.
 * - Registering Neovim Vim commands that delegate to Qt Creator actions
 *   (@c :Build, @c :Run, @c :Debug, etc.).
 * - Intercepting Qt keyboard events and forwarding them as Neovim input.
 * - Bidirectional cursor synchronisation between Qt Creator and Neovim (including
 *   character-wise, line-wise, and block-wise visual selections).
 * - Buffer content synchronisation using a diff-based approach to minimise round trips.
 * - Displaying a Neovim command-line widget in the Qt Creator status bar.
 * - Rendering Vim-style relative line numbers via @c NumbersColumn.
 * - Setting @c $MYQVIMRC and sourcing @c qnvim.vim on startup.
 *
 * ### Threading
 * All public and protected methods must be called from the Qt main thread.
 */
class QNVimCore : public QObject {
    Q_OBJECT
  public:
    /**
     * @brief Constructs the core, spawns Neovim, and connects to Qt Creator's editor manager.
     *
     * A command-line widget is added to the Qt Creator status bar, and the Neovim connector is
     * started with @c --cmd "let g:QNVIM=1".  Once Neovim signals readiness the UI is
     * attached and autocmds / Ex commands that bridge into Qt Creator are registered.
     *
     * @param parent  Optional Qt parent object.
     */
    explicit QNVimCore(QObject *parent = nullptr);

    /**
     * @brief Destroys the core, quits Neovim, and restores Qt Creator to its original state.
     *
     * Removes the command-line widget from the status bar, disconnects editor-manager signals,
     * removes event filters from all open editors, and sends a @c q! command to Neovim.
     */
    virtual ~QNVimCore();

    /**
     * @brief Intercepts Qt events for the attached editor widgets and the command-line widget.
     *
     * Keyboard events are translated to Neovim key sequences and forwarded via the RPC
     * channel.  Resize and focus events trigger geometry or cursor-size updates.
     *
     * @param object  The object that received the event.
     * @param event   The intercepted event.
     * @return @c true if the event was consumed (should not be processed further), @c false
     *         otherwise.
     */
    bool eventFilter(QObject *object, QEvent *event) override;

  protected:
    /**
     * @brief Returns a displayable file path for @p editor.
     *
     * Prefers the on-disk file path; falls back to the editor's display name for untitled
     * buffers.
     *
     * @param editor  The editor to query, or @c nullptr to return an empty string.
     * @return File path string, or an empty string if @p editor is null.
     */
    QString filename(Core::IEditor *editor = nullptr) const;

    /**
     * @brief Resizes the Neovim UI grid to match the current editor's viewport dimensions.
     *
     * Calculates the number of columns and rows that fit inside @p editor's viewport using
     * the editor's font metrics and calls @c nvim_ui_try_resize_grid.
     *
     * @param editor  The editor whose viewport determines the new grid size.  Uses the
     *                current editor if @c nullptr.
     */
    void fixSize(Core::IEditor *editor = nullptr);

    /**
     * @brief Pushes the Qt cursor position to Neovim.
     *
     * No-op in visual modes (cursor position is managed by @c syncSelectionToVim in that
     * case) and when the cursor position has not changed since the last sync.
     *
     * @param editor  The editor to sync.  Uses the current editor if @c nullptr.
     */
    void syncCursorToVim(Core::IEditor *editor = nullptr);

    /**
     * @brief Pushes the Qt text selection to Neovim as a visual-mode selection.
     *
     * Handles character-wise (@c v), line-wise (@c V), and block-wise (@c Ctrl-V) selections
     * as well as Qt Creator's multi-cursor block selections.
     *
     * @param editor  The editor to sync.  Uses the current editor if @c nullptr.
     */
    void syncSelectionToVim(Core::IEditor *editor = nullptr);

    /**
     * @brief Notifies Neovim when the editor's modified state changes.
     *
     * Sets or clears Neovim's buffer-modified flag so that Neovim's @c :w / @c :q! guards
     * reflect the actual document state.
     *
     * @param editor  The editor to sync.  Uses the current editor if @c nullptr.
     */
    void syncModifiedToVim(Core::IEditor *editor = nullptr);

    /**
     * @brief Pushes editor content and cursor to Neovim, then runs an optional callback.
     *
     * Computes a diff between the current Neovim buffer text and the Qt editor text and
     * applies only the changed ranges to minimise RPC traffic.  The optional @p callback is
     * invoked after Neovim acknowledges the update.
     *
     * @param editor    The editor to sync.  Uses the current editor if @c nullptr.
     * @param callback  Optional function to call after the sync completes.
     */
    void syncToVim(Core::IEditor *editor = nullptr, std::function<void()> callback = nullptr);

    /**
     * @brief Updates the Qt cursor from a Neovim cursor position notification.
     *
     * Called from @c handleNotification when Neovim emits @c cursor_goto or equivalent
     * redraw events.  Converts byte-offset coordinates received from Neovim to Qt character
     * positions and updates the editor cursor/selection accordingly.
     *
     * @param pos   Neovim cursor position as @c [line, col] (1-based, byte offsets).
     * @param vPos  Visual anchor position as @c [line, col] (1-based, byte offsets).
     * @param mode  Current Neovim mode byte string (@c "n", @c "v", @c "V", @c "\x16", etc.).
     */
    void syncCursorFromVim(const QVariantList &pos, const QVariantList &vPos, QByteArray mode);

    /**
     * @brief Pulls the current Neovim buffer text and cursor into the active Qt editor.
     *
     * Requests the buffer lines from Neovim via RPC and applies the result to the Qt
     * document using @c QTextCursor.  Only called when Neovim-side changes need to be
     * reflected in Qt Creator (e.g. after an undo, macro playback, or plugin edit).
     */
    void syncFromVim();

    /**
     * @brief Executes a Qt Creator action by its registered command ID.
     *
     * Looks up the action through @c Core::ActionManager and triggers it.  This is the
     * mechanism used by Neovim Ex commands such as @c :Build and @c :Run.
     *
     * @param commandId  The Qt Creator action ID (e.g. @c "ProjectExplorer.Build").
     */
    void triggerCommand(const QByteArray &commandId);

  private slots:
    /**
     * @brief Stores the application cursor flash time without applying it.
     *
     * QNVim disables cursor blinking while active.  This slot captures the user's preferred
     * flash time so it can be restored when QNVim is torn down.
     *
     * @param cursorFlashTime  The cursor flash period in milliseconds.
     */
    void saveCursorFlashTime(int cursorFlashTime);

  private:
    /**
     * @brief Called when an editor becomes the active editor in Qt Creator.
     *
     * Installs an event filter on the editor widget, opens or switches to the corresponding
     * Neovim buffer, and triggers an initial size and content sync.
     *
     * @param editor  The newly active editor.
     */
    void editorOpened(Core::IEditor *editor);

    /**
     * @brief Called just before an editor is closed in Qt Creator.
     *
     * Removes the event filter from the editor widget and cleans up the buffer/editor
     * mapping.  The corresponding Neovim buffer is wiped via @c nvim_buf_delete.
     *
     * @param editor  The editor that is about to be closed.
     */
    void editorAboutToClose(Core::IEditor *editor);

    /**
     * @brief Performs one-time setup for a newly opened Neovim buffer.
     *
     * Subscribes to @c api-buffer-updates for the buffer so that text changes made inside
     * Neovim (e.g. by plugins) are propagated back to Qt Creator.
     *
     * @param bufferId  The Neovim buffer handle returned by @c nvim_get_current_buf.
     */
    void initializeBuffer(int bufferId);

    /**
     * @brief Dispatches an incoming Neovim RPC notification.
     *
     * Routes @c Gui notifications (file auto-commands, cursor updates, trigger-command
     * requests) and @c api-buffer-updates (incremental buffer change events) to the
     * appropriate handler.
     *
     * @param name   The notification channel name (e.g. @c "Gui" or @c "api-buffer-updates").
     * @param args   The notification arguments as a variant list.
     */
    void handleNotification(const QByteArray &name, const QVariantList &args);

    /**
     * @brief Processes a batch of Neovim @c redraw UI events.
     *
     * Iterates over the event list and handles: @c mode_change, @c cursor_goto,
     * @c highlight_set, @c cmdline_show/hide, @c msg_show, @c busy_start/stop, and
     * various option-set events (@c number, @c relativenumber, @c wrap, @c mouse).
     *
     * @param args  The list of redraw sub-event arrays emitted by Neovim.
     */
    void redraw(const QVariantList &args);

    /**
     * @brief Adjusts the visual cursor width in the active editor to reflect the Neovim mode.
     *
     * Sets a block-width cursor in normal/visual mode and a thin cursor in insert mode,
     * matching conventional Vim behaviour.
     */
    void updateCursorSize();

    // -------------------------------------------------------------------------
    // State
    // -------------------------------------------------------------------------

    /// Whether the plugin is currently enabled.
    bool mEnabled = true;

    /// The command-line widget shown in the Qt Creator status bar (mirrors Neovim's cmdline).
    QPlainTextEdit *mCMDLine = nullptr;

    /// The relative-line-number overlay widget for the active editor.
    NumbersColumn *mNumbersColumn = nullptr;

    /// The Neovim connector (owns the child Neovim process).
    NeovimQt::NeovimConnector *mNVim = nullptr;

    /// Incremented each time a change is sent *to* Neovim; used to detect echo-back.
    unsigned mVimChanges = 0;

    /// Maps open Qt Creator editors to their corresponding Neovim buffer IDs.
    QMap<Core::IEditor *, int> mBuffers;

    /// Reverse mapping: Neovim buffer ID → Qt Creator editor.
    QMap<int, Core::IEditor *> mEditors;

    /// Tracks whether each buffer has been modified since the last sync (keyed by buffer ID).
    QMap<int, bool> mChangedTicks;

    /// Stores the Neovim @c buftype option for each buffer ID.
    QMap<int, QString> mBufferType;

    /// Cached plain-text content of the currently active Neovim buffer.
    QString mText;

    /// Current Neovim UI grid width in columns.
    int mWidth = 80;

    /// Current Neovim UI grid height in rows.
    int mHeight = 35;

    /// Neovim foreground colour (from @c hl_attr_define).
    QColor mForegroundColor = Qt::black;

    /// Neovim background colour (from @c hl_attr_define).
    QColor mBackgroundColor = Qt::white;

    /// Neovim special colour (underline / undercurl).
    QColor mSpecialColor;

    /// Cursor foreground colour used to paint the block cursor overlay.
    QColor mCursorColor = Qt::white;

    /// @c true while Neovim has signalled @c busy_start (e.g. during a blocking operation).
    bool mBusy = false;

    /// Whether Neovim's @c mouse option is enabled.
    bool mMouse = false;

    /// Whether Neovim's @c number option is enabled.
    bool mNumber = true;

    /// Whether Neovim's @c relativenumber option is enabled.
    bool mRelativeNumber = true;

    /// Whether Neovim's @c wrap option is enabled.
    bool mWrap = false;

    /// @c true when the Neovim command line is visible (between @c cmdline_show / @c cmdline_hide).
    bool mCMDLineVisible = false;

    /// Raw text currently in the Neovim command line.
    QString mCMDLineContent;

    /// Formatted text displayed in the command-line widget.
    QString mCMDLineDisplay;

    /// Last message shown by Neovim (via @c msg_show).
    QString mMessageLineDisplay;

    /// Cursor position inside the Neovim command-line string.
    int mCMDLinePos = 0;

    /// First character of the command-line prompt (e.g. @c ':' or @c '/').
    QChar mCMDLineFirstc;

    /// Optional prompt string prepended to the command line.
    QString mCMDLinePrompt;

    /// Indentation level of the current command-line prompt.
    int mCMDLineIndent = 0;

    /// Neovim UI mode string (e.g. @c "normal", @c "insert", @c "cmdline_normal").
    QByteArray mUIMode = "normal";

    /// Neovim single-character mode (e.g. @c "n", @c "i", @c "v", @c "V", @c "\x16").
    QByteArray mMode = "n";

    /// Neovim cursor position (1-based column, 1-based line) — x = col, y = line.
    QPoint mCursor;

    /// Neovim visual-anchor position used for selection sync.
    QPoint mVCursor;

    /// Non-zero while Qt Creator is applying a change received from Neovim (prevents re-echo).
    int mSettingBufferFromVim = 0;

    /// Monotonically increasing counter used to correlate async sync operations.
    unsigned long long mSyncCounter = 0;

    /// The cursor flash time saved at startup, restored when QNVimCore is destroyed.
    int mSavedCursorFlashTime = 0;

  signals:
};

} // namespace Internal
} // namespace QNVim
