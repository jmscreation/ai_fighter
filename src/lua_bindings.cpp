#include "lua_bindings.h"
#include "lua_canvas.h"
#include "lua_canvas_entity.h"
#include "debug_point.h"
#include <map>
#include <set>

namespace LuaBindings {

    sol::object CopyObject(sol::object val, sol::state& lua) {
        std::map<const void*, sol::table> tableMap;
        std::function<sol::object(sol::object)> Copy = [&](sol::object value) -> sol::object {            

            switch(value.get_type()){
                case sol::type::boolean: return sol::make_object(lua, value.as<bool>());
                case sol::type::string: return sol::make_object(lua, value.as<std::string>());
                case sol::type::number:{
                    double val = value.as<double>();
                    return fmod(val, 1.0f) == 0.0f ? sol::make_object(lua, int64_t(val)) : sol::make_object(lua, val);
                }
                case sol::type::table:{
                    if(tableMap.count(value.pointer())){
                        return tableMap[value.pointer()].as<sol::object>();
                    }
                    sol::table copy = lua.create_table();
                    tableMap.insert_or_assign(value.pointer(), copy);

                    sol::table table = value.as<sol::table>();
                    for(auto it = table.begin(); it != table.end(); ++it){
                        copy[Copy((*it).first)] = Copy((*it).second);
                    }

                    return copy.as<sol::object>();
                }
                default: return sol::object(sol::nil);
            }
        };

        return Copy(val); // begin recursion
    }

    std::string StringifyObject(sol::object val) {
        std::set<const void*> tableMap;
        std::function<std::string(sol::object)> Stringify = [&](sol::object value) -> std::string {

            switch(value.get_type()){
                case sol::type::userdata: return "<userdata>";
                case sol::type::boolean: return std::to_string(value.as<bool>());
                case sol::type::string: return value.as<std::string>();
                case sol::type::function: return "<function>";
                case sol::type::number:{
                    double val = value.as<double>();
                    return fmod(val, 1.0f) == 0.0f ? std::to_string(int64_t(val)) : std::to_string(val);
                }
                case sol::type::table:{
                    if(tableMap.count(value.pointer())){
                        return "...";
                    }
                    tableMap.emplace(value.pointer());
                    std::string copy = "{ ";
                    sol::table table = value.as<sol::table>();
                    for(auto it = table.begin(); it != table.end(); ++it){
                        std::pair<std::string, std::string> fix, fix2;
                        if((*it).first.get_type() == sol::type::table){
                             fix.first = "[";
                             fix.second = "]";
                        }
                        if((*it).second.get_type() == sol::type::string){
                             fix2.first = "\"";
                             fix2.second = "\"";
                        }
                        copy += (it != table.begin() ? ", " : "")
                            + fix.first + Stringify((*it).first) + fix.second + " = "
                            + fix2.first + Stringify((*it).second) + fix2.second;
                    }
                    copy += "}";

                    return copy;
                }
                default: return "nil";
            }
        };

        return Stringify(val); // begin recursion
    }

    void InitGame(MainApplication::Game& game) {
        sol::state& lua = game.lua;

        lua["cout"] = [](sol::object value){
            std::cout << StringifyObject(value) << "\n";
        };
        
        // User Types

        sol::usertype<olc::vf2d> vector_type = lua.new_usertype<olc::vf2d>("vector", sol::constructors<olc::vf2d(), olc::vf2d(float x, float y)>());
        vector_type["x"] = &olc::vf2d::x;
        vector_type["y"] = &olc::vf2d::y;


        sol::usertype<CanvasProxy> canvas_type = lua.new_usertype<CanvasProxy>("Canvas", sol::constructors<CanvasProxy(uint32_t x, uint32_t y), CanvasProxy(uint32_t x, uint32_t y, uint32_t color)>());
        canvas_type["set"] = [&](CanvasProxy& c, uint32_t x, uint32_t y, uint32_t color){
            if(c.canvas) c.canvas->set(x, y, color);
        };
        canvas_type["fill"] = sol::overload(
            [&](CanvasProxy& c, uint32_t color){
                if(c.canvas) c.canvas->fill(color);
            },
            [&](CanvasProxy& c, uint32_t color, uint32_t x, uint32_t y){
                if(c.canvas) c.canvas->fill(color, x, y);
            },
            [&](CanvasProxy& c, uint32_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height){
                if(c.canvas) c.canvas->fill(color, x, y, width, height);
            }
        );
        canvas_type["get"] = [&](CanvasProxy& c, uint32_t x, uint32_t y){
            if(!c.canvas) return sol::object(sol::nil);
            return sol::make_object(lua, c.canvas->get(x, y));
        };
        canvas_type["width"] = sol::readonly_property(
                        [&](CanvasProxy& c){ return c.canvas ? sol::make_object(lua, c.canvas->width) : sol::object(sol::nil); });
        canvas_type["height"] = sol::readonly_property(
                        [&](CanvasProxy& c){ return c.canvas ? sol::make_object(lua, c.canvas->height) : sol::object(sol::nil); });


        sol::usertype<CanvasEntityProxy> canvas_entity_type = lua.new_usertype<CanvasEntityProxy>("Entity", sol::constructors<CanvasEntityProxy(const CanvasProxy& cnv), CanvasEntityProxy(float x, float y, const CanvasProxy& cnv), CanvasEntityProxy(float x, float y), CanvasEntityProxy()>());
        canvas_entity_type["visible"] = sol::property(
                        [&](CanvasEntityProxy& c, bool b){ if(c.entity) c.entity->visible = b; },
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object(lua, c.entity->visible) : sol::object(sol::nil); });
        canvas_entity_type["x"] = sol::property(
                        [&](CanvasEntityProxy& c, float x){ if(c.entity) c.entity->position.x = x; },
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object(lua, c.entity->position.x) : sol::object(sol::nil); });
        canvas_entity_type["y"] = sol::property(
                        [&](CanvasEntityProxy& c, float y){ if(c.entity) c.entity->position.y = y; },
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object(lua, c.entity->position.y) : sol::object(sol::nil); });
        canvas_entity_type["id"] = sol::readonly_property(
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object(lua, c.entity->getId() ) : sol::object(sol::nil); });

        canvas_entity_type["canvas"] = sol::property(
                        [&](CanvasEntityProxy& c, CanvasProxy& e){
                            if(e.canvas && c.entity) c.entity->set(e.canvas);
                        },
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object(lua, CanvasProxy(std::move(c.entity->get()))) : sol::object(sol::nil); });

        // Bindings

        lua["ScreenWidth"] = [&game](){ return game.window != nullptr ? game.window->ScreenWidth() : game.screen_w; };
        lua["ScreenHeight"] = [&game](){ return game.window != nullptr ? game.window->ScreenHeight() : game.screen_h; };
        lua["ScreenResize"] = [&game](int w, int h){
            if(game.window != nullptr){
                game.window->SetScreenSize(w, h);
            } else {
                game.screen_w = w; game.screen_h = h;
            }
        };

        lua["WindowResize"] = [&](int w, int h){ game.SetWindowSize(w, h); };
        lua["WindowFullscreen"] = [&](bool fullscreen){ game.SetWindowFullscreen(fullscreen); };
        lua["WindowVsync"] = [&](bool vsync){ game.SetWindowVsync(vsync); };

        lua["list_ai"] = [&](){
            sol::table list = lua.create_table();

            for(int i=0; i < game.controllers.size(); ++i){
                AIController* cont = game.controllers[i];
                list[i] = lua.create_table_with("name", cont->name, "controller", sol::make_object_userdata(lua, cont));
            }
            return list;
        };

        lua["find_entity"] = [&](size_t id) {
            std::shared_ptr<CanvasEntity> e = std::dynamic_pointer_cast<CanvasEntity>(Entity::findEntity(id));
            if(!e) return sol::object(sol::nil);

            return sol::make_object(lua, CanvasEntityProxy(std::move(e)));
        };

        // Debug Point Bindings

        lua["add_debug_point"] = [&](float x, float y, uint32_t color){
            return DebugPoint::AddManagedPoint(x, y, 8, 8, color);
        };

        lua["delete_debug_point"] = [&](size_t id){
            DebugPoint::DeletePoint(id);
        };

        lua["move_debug_point"] = [&](size_t id, float x, float y){
            DebugPoint::MovePoint(id, x, y);
        };

        // Deprecated Bindings
/*

        lua["move_entity"] = [&](sol::object entity, float x, float y) {
            std::shared_ptr<Entity> e = entity.as<std::shared_ptr<Entity>>();
            if(e){
                e->position.x = x;
                e->position.y = y;
            }
        };


        lua["destroy_entity"] = [&](sol::object entity) {
            std::shared_ptr<Entity> e = entity.as<std::shared_ptr<Entity>>();
            if(e){
                Entity::destroyEntity(e);
            }
        };

        lua["check_entity_collision"] = [&](sol::object entity) {
            std::shared_ptr<Entity> e = entity.as<std::shared_ptr<Entity>>();
            if(!e) return sol::object(sol::nil);

            sol::table list = lua.create_table();

            std::vector<std::weak_ptr<Entity>> elist;
            if(e->checkCollision(elist)){
                for(int i=0; i < elist.size(); ++i) list[i] = sol::make_object_userdata(lua, elist[i]);
            }

            return list.as<sol::object>();
        };

        lua["get_entity_data"] = [&](sol::object entity) {
            std::shared_ptr<Entity> e = entity.as<std::shared_ptr<Entity>>();
            if(!e) return sol::object(sol::nil);

            sol::table data = lua.create_table_with("collisionBox", lua.create_table());

            data["entity"] = e;
            data["id"] = e->getId();
            data["position"] = e->position;
            data["collisionBox"]["offset"] = e->collisionBox.offset;
            data["collisionBox"]["size"] = e->collisionBox.size;

            return data.as<sol::object>();
        };

        // Old Unit Bindings

        lua["new_unit"] = [&](sol::object controller, float x, float y){
            AIController* cont = controller.as<AIController*>();
            if(cont == nullptr) return sol::object(sol::nil);

            std::shared_ptr<Entity> unit = Unit::createUnit(cont->lua, x, y);

            return sol::make_object_userdata(lua, unit);
        };

        lua["set_unit_color"] = [&](sol::object entity, uint32_t color) {
            std::shared_ptr<Unit> e = std::dynamic_pointer_cast<Unit>(entity.as<std::shared_ptr<Entity>>());
            if(e){
                e->color = color;
            }
        };

        lua["get_unit_color"] = [&](sol::object entity) {
            std::shared_ptr<Unit> e = std::dynamic_pointer_cast<Unit>(entity.as<std::shared_ptr<Entity>>());
            if(!e) return sol::object(sol::nil);
            return sol::make_object(lua, e->color);
        };

        lua["get_unit_instruction"] = [&](sol::object entity){
            std::shared_ptr<Unit> e = std::dynamic_pointer_cast<Unit>(entity.as<std::shared_ptr<Entity>>());
            if(!e) return sol::object(sol::nil);

            Instruction ii = e->popInstruction();
            if(!ii.valid()) return sol::object(sol::nil);

            return lua.create_table_with("type", ii.type, "direction", ii.direction).as<sol::object>();
        };
*/
        // Function Registers / Lua State Transfer

        lua["__registry"] = lua.create_table();

        lua["register"] = [&](const std::string& name, sol::function callback) {
            lua["__registry"][name] = callback;

            for(AIController* cont : game.controllers){
    			sol::state& ai_lua = cont->lua;

                ai_lua.set_function(name, [callback,&ai_lua,&lua](sol::object arg){
                    return CopyObject(callback(CopyObject(arg, lua)), ai_lua);
                });
            }
        };
    }

    void InitAI(AIController& controller, MainApplication::Game& game) {
        sol::state& lua = controller.lua;

        sol::usertype<olc::vf2d> vector_type = lua.new_usertype<olc::vf2d>("vector", sol::constructors<olc::vf2d(), olc::vf2d(float x, float y)>());
        vector_type["x"] = &olc::vf2d::x;
        vector_type["y"] = &olc::vf2d::y;
        /*
        lua["move"] = [&](sol::object entity, const std::string& direction){
            std::shared_ptr<Entity> e = entity.as<std::shared_ptr<Entity>>();
            if(!e) return sol::object(sol::nil);
            std::shared_ptr<Unit> self = std::dynamic_pointer_cast<Unit>(e);
            if(!self) return sol::object(sol::nil);

            int dir = -1;
            if(direction == "right" || direction == "r") dir = 0;
            if(direction == "up" || direction == "u") dir = 1;
            if(direction == "left" || direction == "l") dir = 2;
            if(direction == "down" || direction == "d") dir = 3;

            self->addInstruction({Instruction::Move, dir});

            return entity;
        };

        lua["check_pos"] = [&](float x, float y){
            std::shared_ptr<Entity> e = Entity::pointCheckCollision(x, y);
            if(!e) return sol::object(sol::nil);
            sol::table data = lua.create_table_with("collisionBox", lua.create_table());

            data["id"] = e->getId();
            data["position"] = e->position;
            data["collisionBox"]["offset"] = e->collisionBox.offset;
            data["collisionBox"]["size"] = e->collisionBox.size;

            return data.as<sol::object>();
        };

        lua["get_data"] = [&](sol::object entity) {
            std::shared_ptr<Entity> e = entity.as<std::shared_ptr<Entity>>();
            if(!e) return sol::object(sol::nil);

            sol::table data = lua.create_table_with("collisionBox", lua.create_table());

            data["entity"] = e;
            data["id"] = e->getId();
            data["position"] = e->position;
            data["collisionBox"]["offset"] = e->collisionBox.offset;
            data["collisionBox"]["size"] = e->collisionBox.size;

            return data.as<sol::object>();
        };
        */
    }

}