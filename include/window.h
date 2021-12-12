#pragma once

#include "olcPixelGameEngine.h"
#include "debug_point.h"
#include <functional>

class Window; //forward declarator

typedef std::function<bool (Window*, float)> T_Update;

class Window : public olc::PixelGameEngine {
    T_Update updateFunction;
public:
    Window(T_Update updateFunction, uint32_t w, uint32_t h, uint32_t px = 1, uint32_t py = 1, bool fullscreen=false, bool vsync=true);
    virtual ~Window() = default;
public:
    bool OnUserCreate() override;
    
    bool OnUserUpdate(float elapsedTime) override;
    
    bool OnUserDestroy() override;
};