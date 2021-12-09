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

		sAppName = "Application Name";

		lua.open_libraries(sol::lib::base);
		lua.open_libraries(sol::lib::math);
		
		if(Construct(w, h, px, py)){
			Start();
		}
	}

	Game::~Game() {
		for(AIController* p : controllers){
			delete p;
		}
		controllers.clear();
	}

	bool Game::OnUserCreate() {
		DebugPoint::pge = this; // give debugger pge access
		Entity::initEntitySystem();
		LuaBindings::InitGame(*this);
		lua.set_panic( [](lua_State* state) -> int {
			std::cout << "Lua exception\n";
			return 0;
		});
		
		scripts.loadScript("ai.lua");
		if(!scripts.loadScript("test.lua")) return false;

		lua.safe_script(scripts.getScript("test.lua"), [](lua_State*, sol::protected_function_result pfr){
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

		mainsetup();

		return true;
	};

	bool Game::OnUserUpdate(float delta) {
		if(entRefreshTimer.getSeconds() > 30){
			Entity::optimizeEntities(); // clear out invalid entities
			entRefreshTimer.restart();
		}

		Clear(olc::BLANK);

		drawGrid();

		//std::cout << "Number of instances: " << Entity::entities.size() << "                                     \r";

		mainloop(delta);

		for(std::shared_ptr<Entity> e : Entity::entities) {
			if(!e) continue;

			e->update(delta);
		}

		for(std::shared_ptr<Entity> e : Entity::entities) {
			if(!e) continue;

			e->draw();
		}

		DebugPoint::DrawPoints(); // draw debug points

		std::this_thread::sleep_for(std::chrono::milliseconds(2)); // keep system performance optimal

		return true;
	}

	void Game::drawGrid() {
		for(int x = 0; x < ScreenWidth(); x += 32){
			DrawLine({x, 0}, {x, ScreenHeight()}, 0x0AFFFFFF);
		}

		for(int y = 0; y < ScreenHeight(); y += 32){
			DrawLine({0, y}, {ScreenWidth(), y}, 0x0AFFFFFF);
		}
	}

}



int main(){
	{
		MainApplication::Game app(400, 400);
	}

	return 0;
}