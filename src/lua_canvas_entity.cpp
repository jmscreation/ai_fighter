#include "lua_canvas_entity.h"

CanvasEntity::CanvasEntity(float x, float y): visible(true) {
    position.x = x;
    position.y = y;
    collisionBox.size = {0, 0};
    collisionBox.offset = {0.f, 0.f};
}

CanvasEntity::~CanvasEntity() {
}

std::shared_ptr<CanvasEntity> CanvasEntity::create_entity(float x, float y, const std::shared_ptr<Canvas>& cnv) {
    std::shared_ptr<CanvasEntity> entity = std::make_shared<CanvasEntity>(x, y);
    if(cnv) entity->set(cnv);
    Entity::entities.push_back(std::static_pointer_cast<Entity>(entity));
    return entity;
}

void CanvasEntity::set(std::shared_ptr<Canvas> cnv) {
    canvas = cnv;
    collisionBox.size = {cnv ? float(cnv->width) : 0.f, cnv ? float(cnv->height) : 0.f};
}

std::shared_ptr<Canvas> CanvasEntity::get() {
    return canvas;
}


void CanvasEntity::draw() {
    if(visible && canvas){
        pge->DrawSprite(position + collisionBox.offset, canvas->sprite);
    }
}

// Lua Proxy For CanvasEntity

// Reference Constructor
CanvasEntityProxy::CanvasEntityProxy(std::shared_ptr<CanvasEntity>&& entity): entity(std::move(entity)) {
}

CanvasEntityProxy::CanvasEntityProxy() {
    entity = CanvasEntity::create_entity();
}

CanvasEntityProxy::CanvasEntityProxy(float x, float y) {
    entity = CanvasEntity::create_entity(x, y);
}

CanvasEntityProxy::CanvasEntityProxy(float x, float y, const CanvasProxy& cnv) {
    entity = CanvasEntity::create_entity(x, y, cnv.canvas);
}

CanvasEntityProxy::CanvasEntityProxy(const CanvasProxy& cnv) {
    entity = CanvasEntity::create_entity(0, 0, cnv.canvas);
}

CanvasEntityProxy::~CanvasEntityProxy() {
    Entity::destroyEntity(std::static_pointer_cast<Entity>(entity));
}