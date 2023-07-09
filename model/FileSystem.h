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
    struct Metadata {
        std::string name;
        Type type;
        bool isReadOnly{false};
    } metadata;
    std::string contents;
};
using Files = std::unordered_map<std::string, File>;

class FileSystem {
public:
    // TODO: persistence
    const Files &getFiles() const;

    void deleteFile(const std::string &name);

    void resetFiles();

    void setFiles(const Files &fs);

    void createOrOverwriteFile(const File &file);

private:
    Files _files;
};


#endif //TESTPROJECT_FILESYSTEM_H
