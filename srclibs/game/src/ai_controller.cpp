#include "ai_controller.h"

AIController::AIController(const std::string& str): _name(str), script(_script), name(_name) {
    lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::math);
	lua.open_libraries(sol::lib::string);
}

AIController::~AIController() {
}

void AIController::setScript(const std::string& data){
    _script = data;
}

void AIController::runScript() {
    lua.safe_script(script, [&](lua_State*, sol::protected_function_result pfr){
        sol::error e = pfr;
        std::cout << "Error in " << name << ": " << e.what() << "\n";
        return pfr;
	});
}