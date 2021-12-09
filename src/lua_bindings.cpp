#include "lua_bindings.h"
#include "lua_unit.h"
#include "lua_canvas.h"
#include "lua_canvas_entity.h"
#include "debug_point.h"

namespace LuaBindings {

    std::string StringType(sol::object obj) {
        switch(obj.get_type()){
            case sol::type::string: return obj.as<std::string>();
            case sol::type::number: return std::to_string(obj.as<float>());
            case sol::type::boolean: return obj.as<bool>() ? "true" : "false";
            case sol::type::function: return "function";
            case sol::type::userdata: case sol::type::lightuserdata: return obj.valid() ? "data" : "unknown";
            case sol::type::none: case sol::type::nil: return "nil";
            case sol::type::table : return "table";
        }

        return "unknown";
    }

    std::string StringifyTable(sol::table table, int level=0) {
        std::string str = "{\n";
        table.for_each([&](sol::object key, sol::object value){
            str += std::string(level, '\t') + StringType(key) + " -> " + (value.get_type() == sol::type::table ? StringifyTable(value.as<sol::table>(), level + 1) : StringType(value)) + "\n";
        });
        str += "}\n";
        return str;
    }

    void InitGame(MainApplication::Game& pge) {
        sol::state& lua = pge.lua;

        lua["cout"] = [](sol::object value){
            if(value.get_type() == sol::type::table){
                std::cout << StringifyTable(value.as<sol::table>());
            } else {
                std::cout << StringType(value) << "\n";
            }
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


        sol::usertype<CanvasEntityProxy> canvas_entity_type = lua.new_usertype<CanvasEntityProxy>("CanvasEntity", sol::constructors<CanvasEntityProxy(const CanvasProxy& cnv), CanvasEntityProxy(float x, float y, const CanvasProxy& cnv), CanvasEntityProxy(float x, float y), CanvasEntityProxy()>());
        canvas_entity_type["visible"] = sol::property(
                        [&](CanvasEntityProxy& c, bool b){ if(c.entity) c.entity->visible = b; },
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object(lua, c.entity->visible) : sol::object(sol::nil); });
        canvas_entity_type["x"] = sol::property(
                        [&](CanvasEntityProxy& c, float x){ if(c.entity) c.entity->position.x = x; },
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object(lua, c.entity->position.x) : sol::object(sol::nil); });
        canvas_entity_type["y"] = sol::property(
                        [&](CanvasEntityProxy& c, float y){ if(c.entity) c.entity->position.y = y; },
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object(lua, c.entity->position.y) : sol::object(sol::nil); });
        canvas_entity_type["entity"] = sol::readonly_property(
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object_userdata(lua, std::static_pointer_cast<Entity>(c.entity)) : sol::object(sol::nil); });

        canvas_entity_type["canvas"] = sol::property(
                        [&](CanvasEntityProxy& c, CanvasProxy& e){
                            if(e.canvas && c.entity) c.entity->set(e.canvas);
                        },
                        [&](CanvasEntityProxy& c){ return c.entity ? sol::make_object(lua, CanvasProxy(std::move(c.entity->get()))) : sol::object(sol::nil); });

        // Bindings

        lua["ScreenWidth"] = [&](){ return pge.ScreenWidth(); };
        lua["ScreenHeight"] = [&](){ return pge.ScreenHeight(); };

        lua["add_debug_point"] = [&](float x, float y, uint32_t color){
            return DebugPoint::AddManagedPoint(x, y, 8, 8, color);
        };

        lua["delete_debug_point"] = [&](size_t id){
            DebugPoint::DeletePoint(id);
        };

        lua["move_debug_point"] = [&](size_t id, float x, float y){
            DebugPoint::MovePoint(id, x, y);
        };

        lua["list_ai"] = [&](){
            sol::table list = lua.create_table();

            for(int i=0; i < pge.controllers.size(); ++i){
                AIController* cont = pge.controllers[i];
                list[i] = lua.create_table_with("name", cont->name, "controller", sol::make_object_userdata(lua, cont));
            }
            return list;
        };

        // Core Entity Bindings

        lua["find_entity"] = [&](size_t id) {
            std::shared_ptr<Entity> e = Entity::findEntity(id);
            if(!e) return sol::object(sol::nil);

            return sol::make_object_userdata(lua, e);
        };

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

        // Unit Bindings

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
    }

    void InitAI(AIController& controller, olc::PixelGameEngine& pge) {
        sol::state& lua = controller.lua;

        sol::usertype<olc::vf2d> vector_type = lua.new_usertype<olc::vf2d>("vector", sol::constructors<olc::vf2d(), olc::vf2d(float x, float y)>());
        vector_type["x"] = &olc::vf2d::x;
        vector_type["y"] = &olc::vf2d::y;

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
        
    }

}