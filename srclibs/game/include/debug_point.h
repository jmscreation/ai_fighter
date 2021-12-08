#pragma once

#include "olcPixelGameEngine.h"
#include <vector>

class DebugPoint {
    static std::vector<DebugPoint*> all_points;
    void Init();
public:
    static olc::PixelGameEngine* pge;

    olc::Sprite* sprite;
    olc::Decal texture;
    float x, y;
    const int width, height;
    olc::Pixel color;
public:

    DebugPoint(float x, float y, int width=8, int height=8, olc::Pixel color=olc::WHITE);
    DebugPoint();

    virtual ~DebugPoint();
    
    void draw();
public:
    static size_t AddManagedPoint(float x, float y, int width=8, int height=8, olc::Pixel color=olc::WHITE);
    static void DrawPoints();
    static void ClearPoints();
    static void DeletePoint(size_t id);
    static void MovePoint(size_t id, float x, float y);
};