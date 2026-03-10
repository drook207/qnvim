// SPDX-FileCopyrightText: 2018-2019 Sassan Haradji <sassanh@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <extensionsystem/iplugin.h>
#include <texteditor/texteditor.h>

namespace QNVim {
namespace Internal {

class QNVimCore;
class NumbersColumn;

class QNVimPlugin final : public ExtensionSystem::IPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QNVim.json")

  public:
    QNVimPlugin() = default;

    Utils::Result<> initialize(const QStringList &) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

    bool eventFilter(QObject *, QEvent *) override;

    void toggleQNVim();

  private:
    std::unique_ptr<QNVimCore> m_core;
};

class HelpEditorFactory : public TextEditor::TextEditorFactory {
  public:
    explicit HelpEditorFactory();
};

class TerminalEditorFactory : public TextEditor::TextEditorFactory {
  public:
    explicit TerminalEditorFactory();
};

} // namespace Internal
} // namespace QNVim
