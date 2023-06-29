//
// Created by jolechiw on 6/29/23.
//

#include "DiagnosticsConsumer.h"

void DiagnosticsConsumer::push(const DiagnosticsConsumer::Diagnostic &diagnostic) {
    _diagnostics.push_back(diagnostic);
}

void DiagnosticsConsumer::push(DiagnosticsConsumer::Diagnostic &&diagnostic) {
    _diagnostics.emplace_back(std::move(diagnostic));
}

std::vector<DiagnosticsConsumer::Diagnostic> DiagnosticsConsumer::take() {
    return std::move(_diagnostics);
}
