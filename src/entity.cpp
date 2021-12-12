#include "entity.h"

size_t Entity::counter = 0;
std::vector<std::shared_ptr<Entity>> Entity::entities;

// Entity Static Methods

void Entity::initEntitySystem() {
    entities.reserve(5096);
    counter = 0;
}

void Entity::clearEntities(bool destroy) {
    if(destroy) for(std::shared_ptr<Entity>& e : entities) e.reset();
    entities.clear();
    initEntitySystem();
}

void Entity::optimizeEntities() {
    std::vector<std::shared_ptr<Entity>> _entities(std::move(entities));
    entities.clear();
    entities.reserve(5096);

    for(std::shared_ptr<Entity>& p : _entities){
        if(p) entities.emplace_back(std::move(p));
    }
}

void Entity::destroyEntity(std::shared_ptr<Entity> entity) {
    auto it = std::find(entities.begin(), entities.end(), entity);
    if(it != entities.end()) it->reset();
}

std::shared_ptr<Entity> Entity::pointCheckCollision(float x, float y) {
    olc::vf2d point = {x, y};

    for(std::shared_ptr<Entity> e : entities){
        if(!e) continue;
        olc::vf2d other_topleft = e->position + e->collisionBox.offset,
              other_bottomright = other_topleft + e->collisionBox.size;

        if(other_topleft.x < point.x && other_bottomright.x > point.x &&
           other_topleft.y < point.y && other_bottomright.y > point.y){ // intersects
            return e;
        }
    }

    return nullptr;
}

std::shared_ptr<Entity> Entity::findEntity(size_t id) {
    auto it = std::find_if(entities.begin(), entities.end(), [&](std::shared_ptr<Entity> e) -> bool {
        if(!e) return false;
        return e->id == id;
    });

    if(it == entities.end()) return nullptr;
    return *it;
}

// Entity Methods

Entity::Entity(): position({0,0}), collisionBox({{-16, -16}, {32, 32}}) {
    id = counter++;
}

Entity::~Entity() {}

void Entity::draw() { }
