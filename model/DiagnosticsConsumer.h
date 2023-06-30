//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_DIAGNOSTICSCONSUMER_H
#define TESTPROJECT_DIAGNOSTICSCONSUMER_H

#include <vector>
#include <string>
#include <mutex>

class DiagnosticsConsumer {
public:
    DiagnosticsConsumer() = default;

    enum class Type {
        System, User
    };
    enum class Level {
        Error, Warning, Info, Debug
    };
    struct Diagnostic {
        Type type;
        Level level;
        std::string title;
        std::string message;
    };

    std::vector<Diagnostic> take();
    void push(const Diagnostic &diagnostic);
private:
    std::vector<Diagnostic> _diagnostics;
    std::mutex _lock;
};


#endif //TESTPROJECT_DIAGNOSTICSCONSUMER_H
