#pragma once

#include "window.h"
#include <limits>
#include "sol/sol.hpp"
#include "ai_controller.h"
#include "clock.h"
#include <iostream>
#include <vector>
#include <functional>

namespace MainApplication {
    class Game {
		bool windowChanged, gameStartup;
	public:
		uint32_t screen_w, screen_h, px, py;
		bool vsync, fullscreen;

		Window* window;

		Game(uint32_t w, uint32_t h, uint32_t px = 1, uint32_t py = 1);
		virtual ~Game();

		bool OpenWindow();

		void SetWindowSize(int w, int h);
		void SetWindowFullscreen(bool bFullscreen);
		void SetWindowVsync(bool vSync);

		bool OnUserUpdate(Window* pge, float delta);
		bool OnUserCreate(Window* pge);

	public:
		static sol::state* _lua;
		sol::state& lua;

		std::vector<AIController*> controllers;
		Clock entRefreshTimer;
	};
}