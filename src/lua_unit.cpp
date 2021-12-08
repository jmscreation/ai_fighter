#include "lua_unit.h"

Unit::Unit(sol::state& lua): lua(lua) {
    maxInstructions = 10;
    updateFunction = lua["unit_loop"];
    mainUpdateFunction = MainApplication::lua["unit_loop"];
}

Unit::~Unit() {
}

bool Unit::addInstruction(Instruction ii) {
    if(instructions.size() >= maxInstructions) return false;
    instructions.push(ii);
    return true;
}

Instruction Unit::popInstruction() {
    Instruction ii {};
    if(!instructions.size()) return ii;

    ii = instructions.front();
    instructions.pop();

    return ii;
}

std::shared_ptr<Entity> Unit::createUnit(sol::state& lua, float x, float y) {
    std::shared_ptr<Entity> e = std::static_pointer_cast<Entity>(std::make_shared<Unit>(lua));
    entities.push_back(e);

    e->position = {x, y};

    return e;
}

void Unit::draw() {
    pge->FillRect(position + collisionBox.offset, collisionBox.size, color);
}

void Unit::update(float delta) {
    Entity::update(delta);
    updateFunction(getPointer(), delta);
    mainUpdateFunction(getPointer(), delta);
}