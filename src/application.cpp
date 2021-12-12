#include "application.h"
#include "entity.h"
#include "lua_bindings.h"
#include "scriptloader.h"
#include "debug_point.h"

namespace MainApplication {
	ScriptLoader scripts;
	sol::function mainsetup, mainloop;
	
	sol::state* Game::_lua = nullptr;

	Game::Game(uint32_t w, uint32_t h, uint32_t px, uint32_t py): lua(*(new sol::state)) {
		_lua = &lua;
		window = nullptr;

		screen_w = w;
		screen_h = h;
		this->px = px;
		this->py = py;
		this->vsync = vsync;
		this->fullscreen = fullscreen;
		windowChanged = true;

		lua.open_libraries(sol::lib::base);
		lua.open_libraries(sol::lib::math);
		lua.open_libraries(sol::lib::package);
		lua.open_libraries(sol::lib::string);

		Entity::initEntitySystem();
		LuaBindings::InitGame(*this);
		
		scripts.loadScript("ai.lua");
		if(!scripts.loadScript("game.lua")){
			std::cout << "Failed to load \"game.lua\" game script\n";
			return;
		}

		lua.safe_script(scripts.getScript("game.lua"), [](lua_State*, sol::protected_function_result pfr){
			sol::error e = pfr;
			std::cout << "error: " << e.what() << "\n";
			return pfr;
		});

		mainsetup = lua["setup"];
		mainloop = lua["loop"];
		
		{ // temp
			AIController* cont = new AIController("ai_1");
			cont->setScript(scripts.getScript("ai.lua"));

			controllers.push_back(cont);
		}

		for(AIController* cont : controllers){
			LuaBindings::InitAI(*cont, *this);
			cont->runScript();
		}

		// Main lua environment has now loaded

		mainsetup(); // run game setup

		for(AIController* cont : controllers){
			cont->lua["setup"](); // run ai setup
		}
		
		while(OpenWindow());
	}

	Game::~Game() {
		for(AIController* p : controllers){
			delete p;
		}
		controllers.clear();
	}

	bool Game::OpenWindow() {
		if(!windowChanged) return false;
		windowChanged = false;
		window = new Window(std::bind(&Game::OnUserUpdate, this, std::placeholders::_1, std::placeholders::_2),
							screen_w, screen_h, px, py, fullscreen, vsync);
		delete window;
		window = nullptr;

		return windowChanged;
	}

	void Game::SetWindowSize(int w, int h) {
		screen_w = w;
		screen_h = h;
		windowChanged = true;
	}

	void Game::SetWindowFullscreen(bool bFullscreen) {
		fullscreen = bFullscreen;
		windowChanged = true;
	}

	void Game::SetWindowVsync(bool bVsync) {
		vsync = bVsync;
		windowChanged = true;
	}

	bool Game::OnUserUpdate(Window* pge, float delta) {
		if(entRefreshTimer.getSeconds() > 30){
			Entity::optimizeEntities(); // clear out invalid entities
			entRefreshTimer.restart();
		}

		pge->Clear(olc::BLANK);

		//std::cout << "Number of instances: " << Entity::entities.size() << "                                     \r";

		mainloop(delta);

		for(AIController* cont : controllers){
			if(cont != nullptr) cont->lua["loop"](delta);
		}

		for(std::shared_ptr<Entity> e : Entity::entities){
			if(!e) continue;

			e->draw();
		}

		DebugPoint::DrawPoints(); // draw debug points

		if(windowChanged) return false;

		std::this_thread::sleep_for(std::chrono::milliseconds(2)); // keep system performance optimal

		return true;
	}
}



int main(){
	{
		MainApplication::Game app(400, 400);
	}

	return 0;
}