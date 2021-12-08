#pragma once

#include "application.h"
#include "entity.h"

class Canvas : public Entity {
    olc::Sprite* sprite;
    void Init(uint32_t color);

public:
    const uint32_t width, height;
    bool visible;

    static std::shared_ptr<Canvas> create_canvas(uint32_t width, uint32_t height, uint32_t color=0x00000000);

    Canvas(uint32_t width, uint32_t height, uint32_t color);
    virtual ~Canvas();


    void set(uint32_t x, uint32_t y, uint32_t color);
    uint32_t get(uint32_t x, uint32_t y);

    void draw() override;
};

struct CanvasProxy {
    std::shared_ptr<Canvas> canvas;

    CanvasProxy(uint32_t width, uint32_t height);
    CanvasProxy(uint32_t width, uint32_t height, uint32_t color);
    virtual ~CanvasProxy();
};