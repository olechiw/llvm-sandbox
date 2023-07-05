//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_DIAGNOSTICS_H
#define TESTPROJECT_DIAGNOSTICS_H

#include <vector>
#include <string>
#include <mutex>

class Diagnostics {
public:
    Diagnostics() = default;

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


#endif //TESTPROJECT_DIAGNOSTICS_H
