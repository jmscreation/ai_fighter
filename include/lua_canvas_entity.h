#pragma once

#include "application.h"
#include "entity.h"
#include "lua_canvas.h"

class CanvasEntity : public Entity {
    std::shared_ptr<Canvas> canvas;
public:
    bool visible;

    static std::shared_ptr<CanvasEntity> create_entity(float x=0, float y=0, const std::shared_ptr<Canvas>& cnv = nullptr);

    CanvasEntity(float width, float height);
    virtual ~CanvasEntity();

    void set(std::shared_ptr<Canvas> cnv);
    std::shared_ptr<Canvas> get();

    void draw() override;

};

struct CanvasEntityProxy {
    std::shared_ptr<CanvasEntity> entity;

    CanvasEntityProxy(std::shared_ptr<CanvasEntity>&& entity); // reference constructor

    CanvasEntityProxy();
    CanvasEntityProxy(float x, float y);
    CanvasEntityProxy(const CanvasProxy& cnv);
    CanvasEntityProxy(float x, float y, const CanvasProxy& cnv);
    virtual ~CanvasEntityProxy();

};