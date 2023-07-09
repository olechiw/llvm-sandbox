//
// Created by jolechiw on 6/29/23.
//

#include "DiagnosticsView.h"

DiagnosticsView::DiagnosticsView(Diagnostics &diagnostics) : _diagnostics(diagnostics) {
    _textEditor.SetReadOnly(true);
    _textEditor.SetText("");
}

void DiagnosticsView::render() {
    auto diagnostics = _diagnostics.take();

    if (!diagnostics.empty()) {
        for (const auto &diag : diagnostics) {
            std::string logLine;
            switch (diag.type) {
                case Diagnostics::Type::System:
                    logLine += "[SYS]";
                    break;
                case Diagnostics::Type::User:
                    logLine += "[USR]";
                    break;
            }
            switch (diag.level) {
                case Diagnostics::Level::Error:
                    logLine += "[ERR]";
                    break;
                case Diagnostics::Level::Warning:
                    logLine += "[WRN]";
                    break;
                case Diagnostics::Level::Info:
                    logLine += "[INF]";
                    break;
                case Diagnostics::Level::Debug:
                    logLine += "[DBG]";
                    break;
            }
            logLine += " " + diag.title;
            if (!diag.message.empty())
                logLine += "\n\t\t" + diag.message;
            logLine += "\n";

            _textEditor.SetReadOnly(false);
            _textEditor.MoveBottom();
            _textEditor.InsertText(logLine);
            _textEditor.SetReadOnly(true);
        }
    }

    ImGui::Text("Diagnostics:");
    _textEditor.Render("DiagnosticsView");
}
