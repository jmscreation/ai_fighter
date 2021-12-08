#include "scriptloader.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

std::string ScriptLoader::nilscript;

ScriptLoader::ScriptLoader() {}

ScriptLoader::~ScriptLoader() {}

bool ScriptLoader::loadScript(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if(!file.is_open() || file.bad()) return false;

    std::string data;
    std::streamsize read;
    do {
        char chunk[1024];
        read = file.readsome(chunk, sizeof(chunk));
        if(read) data.append(chunk, read);
    } while(read != 0);
    std::string name = fs::path(path).filename().string();

    rawfiles.insert_or_assign(name, std::move(data));

    return true;
}