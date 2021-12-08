#pragma once

#include "application.h"
#include "entity.h"
#include <queue>

struct Instruction {
    enum Type {
        Null, Move, Attack, Defend
    } type;
    int direction;

    inline bool valid() { return type != Null; }
};

class Unit : public Entity {
    sol::state& lua;
    sol::function updateFunction, mainUpdateFunction;

    size_t maxInstructions;
    std::queue<Instruction> instructions;

public:
    static std::shared_ptr<Entity> createUnit(sol::state& lua, float x, float y);

    Unit(sol::state& lua);
    virtual ~Unit();

    bool addInstruction(Instruction ii);
    Instruction popInstruction();

    virtual void draw() override;
    virtual void update(float delta) override;
};