#include "lua_canvas.h"

Canvas::Canvas(uint32_t width, uint32_t height, uint32_t color): width(width), height(height) {
    Init(color);
}

void Canvas::Init(uint32_t color) {
    visible = false;

    collisionBox.size = {float(width), float(height)};
    collisionBox.offset = {0.f, 0.f};

    sprite = new olc::Sprite(width, height);
    for(olc::Pixel& p : sprite->pColData) p.n = color; // fill
}

Canvas::~Canvas() {
    delete sprite;
}

std::shared_ptr<Canvas> Canvas::create_canvas(uint32_t width, uint32_t height, uint32_t color) {
    std::shared_ptr<Canvas> canvas = std::make_shared<Canvas>(width, height, color);
    Entity::entities.push_back(canvas->getPointer()); // pushback entity
    return canvas;
}

void Canvas::draw() {
    if(visible){
        pge->DrawSprite(position + collisionBox.offset, sprite);
    }
}

void Canvas::set(uint32_t x, uint32_t y, uint32_t color) {
    sprite->SetPixel(x, y, color);
}

uint32_t Canvas::get(uint32_t x, uint32_t y) {
    return sprite->GetPixel(x, y).n;
}


// Lua Proxy For Canvas Entity

CanvasProxy::CanvasProxy(uint32_t width, uint32_t height) {
    canvas = Canvas::create_canvas(width, height);
}

CanvasProxy::CanvasProxy(uint32_t width, uint32_t height, uint32_t color) {
    canvas = Canvas::create_canvas(width, height, color);
}


CanvasProxy::~CanvasProxy() {
    canvas.reset();
}