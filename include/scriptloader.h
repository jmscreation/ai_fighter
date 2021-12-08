#pragma once

#include <fstream>
#include <map>
#include <string>
#include <cassert>

// script loader system

class ScriptLoader {
    static std::string nilscript;
    std::map<std::string, std::string> rawfiles;
public:
    ScriptLoader();
    virtual ~ScriptLoader();

    bool loadScript(const std::string& path);
    inline const std::string& getScript(const std::string& name) {
        if(!rawfiles.count(name)) return nilscript;
        return rawfiles[name];
    }
};