//
// Created by jolechiw on 7/5/23.
//

#include "FileSystem.h"


const Files &FileSystem::getFiles() const {
    return _files;
}

void FileSystem::createOrOverwriteFile(const File &file) {
    _files[file.metadata.name] = file;
}

void FileSystem::deleteFile(const std::string &name) {
    _files.erase(name);
}

void FileSystem::resetFiles() {
    _files.clear();
}

void FileSystem::setFiles(const Files &fs) {
    _files = fs;
}