#pragma once

#include "application.h"
#include "olcPixelGameEngine.h"

namespace LuaBindings {

    extern sol::object CopyObject(sol::object value, sol::state& lua);

    extern void InitGame(MainApplication::Game& pge);
    extern void InitAI(AIController& controller, olc::PixelGameEngine& pge);

}