#pragma once

#include "application.h"
#include "olcPixelGameEngine.h"

namespace LuaBindings {

    extern sol::object CopyObject(sol::object value, sol::state& lua);

    extern void InitGame(MainApplication::Game& game);
    extern void InitAI(AIController& controller, MainApplication::Game& game);

}