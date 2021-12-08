#pragma once

#include "olcPixelGameEngine.h"
#include <limits>
#include "sol/sol.hpp"
#include "ai_controller.h"
#include <iostream>
#include <vector>

namespace MainApplication {
    extern sol::state lua;

    class Game : public olc::PixelGameEngine {

		void drawGrid();

	public:
		Game(uint32_t w, uint32_t h, uint32_t px = 1, uint32_t py = 1);
		virtual ~Game();
	public:
		std::vector<AIController*> controllers;

		bool OnUserCreate();
		
		bool OnUserUpdate(float elapsedTime);
		
	};
}