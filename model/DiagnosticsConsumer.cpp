//
// Created by jolechiw on 6/29/23.
//

#include "DiagnosticsConsumer.h"
void DiagnosticsConsumer::push(const DiagnosticsConsumer::Diagnostic &diagnostic) {
    std::unique_lock<std::mutex> raii(_lock);
    _diagnostics.push_back(diagnostic);
}

std::vector<DiagnosticsConsumer::Diagnostic> DiagnosticsConsumer::take() {
    std::unique_lock<std::mutex> raii(_lock);
    return std::move(_diagnostics);
}
