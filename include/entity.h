#pragma once

#include "application.h"
#include "olcPixelGameEngine.h"
#include <vector>
#include <memory>

class Entity : public olc::PGEX , public std::enable_shared_from_this<Entity> {
    static size_t counter;

    size_t id;

protected:
    static std::vector<std::shared_ptr<Entity>> entities;

    Entity();
public:
    olc::vf2d position; // cartesian point in space

    struct {
        olc::vf2d offset; // offset from position
        olc::vf2d size;
    } collisionBox;

    virtual ~Entity();

    inline std::shared_ptr<Entity> getPointer() { return shared_from_this(); }
    inline size_t getId() { return id; }

    virtual void draw();

    size_t checkCollision(std::vector<std::weak_ptr<Entity>>& list);

public: // static methods
    static void initEntitySystem();
    static void optimizeEntities();
    static void clearEntities(bool destroy=true);
    static void destroyEntity(std::shared_ptr<Entity> e);
    static std::shared_ptr<Entity> pointCheckCollision(float x, float y);
    static std::shared_ptr<Entity> findEntity(size_t id);


friend class MainApplication::Game;

};