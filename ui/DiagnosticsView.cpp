//
// Created by jolechiw on 6/29/23.
//

#include "DiagnosticsView.h"

DiagnosticsView::DiagnosticsView(Context &context) : _context(context) {
    _textEditor.SetReadOnly(true);
    _textEditor.SetText("");
}

void DiagnosticsView::render() {
    auto diagnostics = _context.getDiagnosticsConsumer().take();

    if (!diagnostics.empty()) {
        for (const auto &diag : diagnostics) {
            std::string logLine;
            switch (diag.type) {
                case DiagnosticsConsumer::Type::System:
                    logLine += "[SYS]";
                    break;
                case DiagnosticsConsumer::Type::User:
                    logLine += "[USR]";
                    break;
            }
            switch (diag.level) {
                case DiagnosticsConsumer::Level::Error:
                    logLine += "[ERR]";
                    break;
                case DiagnosticsConsumer::Level::Warning:
                    logLine += "[WRN]";
                    break;
                case DiagnosticsConsumer::Level::Info:
                    logLine += "[INF]";
                    break;
                case DiagnosticsConsumer::Level::Debug:
                    logLine += "[DBG]";
                    break;
            }
            logLine += " " + diag.title;
            if (!diag.message.empty())
                logLine += "\n\t\t" + diag.message;
            logLine += "\n";

            // TODO: Test this later
            _textEditor.SetReadOnly(false);
            _textEditor.MoveBottom();
            _textEditor.InsertText(logLine);
            _textEditor.SetReadOnly(true);
        }
    }

    ImGui::Text("Diagnostics:");
    _textEditor.Render("DiagnosticsView");
}
