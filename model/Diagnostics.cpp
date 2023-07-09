//
// Created by jolechiw on 6/29/23.
//

#include "Diagnostics.h"

void Diagnostics::push(const Diagnostics::Diagnostic &diagnostic) {
    std::unique_lock<std::mutex> raii(_lock);
    _diagnostics.push_back(diagnostic);
}

std::vector<Diagnostics::Diagnostic> Diagnostics::take() {
    std::unique_lock<std::mutex> raii(_lock);
    return std::move(_diagnostics);
}
