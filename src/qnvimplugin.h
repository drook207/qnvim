// SPDX-FileCopyrightText: 2018-2019 Sassan Haradji <sassanh@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <extensionsystem/iplugin.h>
#include <texteditor/texteditor.h>

namespace QNVim {
namespace Internal {

class QNVimCore;
class NumbersColumn;

/**
 * @class QNVimPlugin
 * @brief Qt Creator plugin entry point for QNVim.
 *
 * @c QNVimPlugin is the top-level @c IPlugin subclass registered with Qt Creator's
 * extension system.  It is responsible for:
 * - Registering the @e Toggle QNVim action and the @e QNVim top-level menu entry.
 * - Creating and owning the @c QNVimCore instance that drives the actual Neovim
 *   integration.
 * - Forwarding Qt events from the main window to @c QNVimCore.
 * - Providing the @c toggleQNVim() slot so the user can enable/disable the plugin
 *   at runtime without restarting Qt Creator.
 *
 * The default keyboard shortcut for toggling is @c Alt+Shift+V, Alt+Shift+V.
 */
class QNVimPlugin final : public ExtensionSystem::IPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QNVim.json")

  public:
    QNVimPlugin() = default;

    /**
     * @brief Initialises the plugin: registers actions, the menu, and creates @c QNVimCore.
     *
     * Also unsets the @c NVIM_LISTEN_ADDRESS environment variable to prevent a running
     * Neovim instance from interfering with the embedded one.
     *
     * @param arguments  Command-line arguments passed by Qt Creator (currently unused).
     * @return An empty @c Utils::Result<> on success, or an error result on failure.
     */
    Utils::Result<> initialize(const QStringList &arguments) override;

    /**
     * @brief Called after all dependent plugins are fully initialised.
     *
     * Currently a no-op; reserved for future cross-plugin interactions.
     */
    void extensionsInitialized() override;

    /**
     * @brief Called before Qt Creator shuts down; destroys @c QNVimCore.
     *
     * @return @c SynchronousShutdown because teardown completes immediately.
     */
    ShutdownFlag aboutToShutdown() override;

    /**
     * @brief Delegates event filtering to @c QNVimCore when the plugin is active.
     *
     * @param object  The watched object.
     * @param event   The intercepted event.
     * @return The result of @c QNVimCore::eventFilter, or @c false if the core is null.
     */
    bool eventFilter(QObject *object, QEvent *event) override;

    /**
     * @brief Toggles QNVim on or off at runtime.
     *
     * When active, destroys @c QNVimCore (disconnecting Neovim and restoring Qt Creator's
     * default behaviour).  When inactive, creates a fresh @c QNVimCore.
     */
    void toggleQNVim();

  private:
    /// Owns the core integration object; null when QNVim is disabled.
    std::unique_ptr<QNVimCore> m_core;
};

/**
 * @class HelpEditorFactory
 * @brief Text editor factory that handles Qt Creator's built-in @e Help editor type.
 *
 * Registers the @c "Help" editor ID with a @c text/plain MIME type so that QNVim can
 * intercept help-page buffers and route them through the Neovim buffer pipeline.
 */
class HelpEditorFactory : public TextEditor::TextEditorFactory {
  public:
    /// Constructs the factory and registers the @c "Help" editor ID.
    explicit HelpEditorFactory();
};

/**
 * @class TerminalEditorFactory
 * @brief Text editor factory that handles Qt Creator's integrated terminal editor type.
 *
 * Registers the @c "Terminal" editor ID with a @c text/plain MIME type so that QNVim can
 * interact with terminal buffers consistently.
 */
class TerminalEditorFactory : public TextEditor::TextEditorFactory {
  public:
    /// Constructs the factory and registers the @c "Terminal" editor ID.
    explicit TerminalEditorFactory();
};

} // namespace Internal
} // namespace QNVim
