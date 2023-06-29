//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_DIAGNOSTICSCONSUMER_H
#define TESTPROJECT_DIAGNOSTICSCONSUMER_H

#include <vector>
#include <string>

class DiagnosticsConsumer {
public:
    DiagnosticsConsumer() = default;
    DiagnosticsConsumer(DiagnosticsConsumer &&) = default;
    DiagnosticsConsumer(const DiagnosticsConsumer &) = delete;

    enum class Type {
        System, User
    };
    enum class Level {
        Error, Warning, Info
    };
    struct Diagnostic {
        Type type;
        Level level;
        std::string title;
        std::string message;
    };

    std::vector<Diagnostic> take() { return std::move(_diagnostics); }
    void push(const Diagnostic &diagnostic) {
        _diagnostics.push_back(diagnostic);
    }

    void push(Diagnostic &&diagnostic) {
        _diagnostics.emplace_back(std::move(diagnostic));
    }
private:
    std::vector<Diagnostic> _diagnostics;
};


#endif //TESTPROJECT_DIAGNOSTICSCONSUMER_H
