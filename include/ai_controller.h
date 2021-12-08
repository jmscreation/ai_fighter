#pragma once

#include <limits>
#include "sol/sol.hpp"
#include <string>

class AIController {
    std::string _script, _name;
public:
    const std::string& script;
    const std::string& name;

    sol::state lua;

    AIController(const std::string& str="no-name");
    virtual ~AIController();

    void setScript(const std::string& data);
    void runScript();
};