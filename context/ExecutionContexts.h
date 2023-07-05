//
// Created by jolechiw on 7/5/23.
//

#ifndef TESTPROJECT_EXECUTIONCONTEXTS_H
#define TESTPROJECT_EXECUTIONCONTEXTS_H

#include <string>
#include <unordered_map>
#include <iostream>

class ExecutionContexts {
public:
    struct T {
        std::unordered_map<std::string, std::string> helperFiles;
        std::unordered_map<std::string, std::string> starterFiles;
        std::unordered_map<std::string, void*> dynamicLibraries;
    };
    static const T &getHelloWorld();
};


#endif //TESTPROJECT_EXECUTIONCONTEXTS_H
