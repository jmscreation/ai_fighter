#pragma once

#include "application.h"
#include "olcPixelGameEngine.h"

namespace LuaBindings {

    extern void InitGame(MainApplication::Game& pge);
    extern void InitAI(AIController& controller, olc::PixelGameEngine& pge);

}