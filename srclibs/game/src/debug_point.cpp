#include "debug_point.h"

olc::PixelGameEngine* DebugPoint::pge = nullptr;
std::vector<DebugPoint*> DebugPoint::all_points;

DebugPoint::DebugPoint(float x, float y, int width, int height, olc::Pixel color):
    sprite(new olc::Sprite(width, height)),
    texture(sprite), x(x), y(y), width(width), height(height), color(color) {

    Init();
}

DebugPoint::DebugPoint():
    sprite(new olc::Sprite(8, 8)), texture(sprite), x(0), y(0), width(8), height(8), color(olc::WHITE) {

    Init();
}

DebugPoint::~DebugPoint() {
    delete sprite;
}

void DebugPoint::Init() {
    pge->SetDrawTarget(sprite);
    pge->Clear(olc::BLANK);
    pge->DrawCircle(width/2,height/2,std::min(floor(width/4)*2 - 1, floor(height/4)*2 - 1));
    pge->SetDrawTarget(nullptr);

    texture.Update();
}

void DebugPoint::draw() {
    if(pge != nullptr) pge->DrawDecal({x - width / 2, y - height / 2}, &texture, {1.0f,1.0f}, color);
}





size_t DebugPoint::AddManagedPoint(float x, float y, int width, int height, olc::Pixel color) {
    DebugPoint* p = new DebugPoint(x, y, width, height, color);
    all_points.push_back(p);
    return all_points.size() - 1;
}

void DebugPoint::DeletePoint(size_t id) {
    if(id >= all_points.size()) return;
    DebugPoint*& p = all_points[id]; // modify internal array
    delete p;
    p = nullptr;
}

void DebugPoint::MovePoint(size_t id, float x, float y) {
    if(id >= all_points.size()) return;
    DebugPoint* p = all_points[id]; // modify internal array
    if(p == nullptr) return;

    p->x = x;
    p->y = y;
}

void DebugPoint::ClearPoints() {
    for(DebugPoint* p : all_points) delete p;
    all_points.clear(); // clear list
}

void DebugPoint::DrawPoints() {
    for(DebugPoint* p : all_points){
        if(p != nullptr) p->draw();
    }
}