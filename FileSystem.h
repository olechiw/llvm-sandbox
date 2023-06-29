//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_FILESYSTEM_H
#define TESTPROJECT_FILESYSTEM_H

#include <unordered_map>
#include <string>

struct File {
    enum class Type {
        CPP, H
    };

    std::string name;
    std::string contents;
    Type type;
    bool isReadOnly { false };
};
using FileSystem = std::unordered_map<std::string, File>;


#endif //TESTPROJECT_FILESYSTEM_H
