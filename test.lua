-- global variables
game_timer = 0;
entity_table = {};
ai = {};


-- mini lib functions
function sign(x)
    return x>0 and 1 or x<0 and -1 or 0
end
function near(x1,x2)
    return math.abs(x2-x1) < 1;
end

--game functions
function check_position(me, x, y)
    for k, entity in pairs(entity_table) do
        if me.id == entity.id then goto continue; end

        local obj = find_entity(entity.id);
        if obj == nil then goto continue; end

        if (entity.tx == x and entity.ty == y) or (entity.fx == x and entity.fy == y) then
            return true;
        end
        ::continue::
    end

    return false;
end

--per instance loop
function unit_loop(obj, delta)
    local self = get_entity_data(obj);
    local vx = 0;
    local vy = 0;
    if entity_table[self.id] == nil then
        self.life = 2;
        self.instruction = nil;
        self.tx = self.position.x;
        self.ty = self.position.y;
        self.fx = self.tx;
        self.fy = self.ty;
        --self.dbg_point = add_debug_point(self.tx, self.ty, 0xFFFF000A);
        entity_table[self.id] = self;
    else
        self = entity_table[self.id];
    end
    
    if self.instruction == nil then
        self.instruction = get_unit_instruction(obj);
        if self.instruction ~= nil then
            self.instruction.used = false;
        end
    end

    if self.instruction ~= nil then
        local ii = self.instruction;        
        
        if not ii.used then
            ii.used = true;
            local dir = ii.direction;
            if ii.type == 0 then -- null
                self.instruction = nil;
            elseif ii.type == 1 then --move
                if dir == -1 then self.instruction = nil; end
                local tx = self.tx;
                local ty = self.ty;
                if dir == 0 or dir == 2 then
                    tx = self.position.x + (dir == 0 and 32 or -32);
                else
                    ty = self.position.y + (dir == 1 and -32 or 32);
                end
                if check_position(self, tx, ty) then
                    self.instruction = nil;
                else
                    -- Clamp the tx and ty to a 32x32 grid within game room
                    self.tx = math.max(math.min(math.floor(ScreenWidth() / 32) * 32, tx), 0);
                    self.ty = math.max(math.min(math.floor(ScreenHeight() / 32) * 32, ty), 0);
                end
            elseif ii.type == 2 then --attack
            elseif ii.type == 3 then --defend
            end
        end
    end
    
    if not near(self.position.x, self.tx) or not near(self.position.y, self.ty) then
        vx = sign(self.tx - self.position.x) * 48;
        vy = sign(self.ty - self.position.y) * 48;
    else
        vx = 0;
        vy = 0;
        self.position.x = self.tx;
        self.position.y = self.ty;
        self.fx = self.tx;
        self.fy = self.ty;
        self.instruction = nil;
    end

    self.position.x = self.position.x + vx * delta;
    self.position.y = self.position.y + vy * delta;
    
    if self.position.x > 400 or self.position.x < 0 then
        self.position.x = math.max(math.min(ScreenWidth(), self.position.x), 0);
    end
    if self.position.y > 400 or self.position.y < 0 then
        self.position.y = math.max(math.min(ScreenHeight(), self.position.y), 0);
    end

    move_entity(obj, self.position.x, self.position.y);
    --move_debug_point(self.dbg_point, self.fx, self.fy);
    
    if self.life < 0 then
        destroy_entity(obj);
    end
end


function add_unit(controller, xx, yy, color)
    local u = new_unit(controller, math.floor(xx / 32) * 32, math.floor(yy / 32) * 32); -- align to grid
    set_unit_color(u, color);
end


-- main setup function
cents = {};

closure = nil;

function setup()
    print("Initialize...\n");

    local canvas = Canvas.new(128, 128, 0xFFFFFFFF);
    canvas:fill(0xFFFF0000, 32, 32);
    canvas:fill(0xFF00FF00, 32, 32, 64, 32);
    
    cents[0] = CanvasEntity.new(canvas);
    cents[1] = CanvasEntity.new(200, 64, canvas);

    local test = CanvasEntity.new(Canvas.new(500, 500, 0xFFAB000F));
    test.x = 64;
    test.y = 64;

    closure = function ()
        test.x = 0;
        test.y = 1;
        test.canvas = cents[1].canvas;
    end

    move_entity(cents[0].entity, 64, 64);
    
    ai = list_ai();
    for k,v in pairs(ai) do
        for i=0, 5 do
            add_unit(v.controller,
            (k < 1 and 32 or ScreenWidth() - 32), 32 + 64 * i,
            0xFF0000FF - math.floor(math.random() * 0xA0));
        end
    end
end

-- main game loop function
clock = 0;
function loop(delta)
    game_timer = game_timer + delta;

    c = cents[0];
    if c ~= nil then
        c.canvas:set(
            math.floor(math.random() * c.canvas.width),
            math.floor(math.random() * c.canvas.height),
            0xFF000000);
    end

    if game_timer - clock > 5 then
        if cents[0] == nil then
            closure = nil;
        else
            closure();
        end
        cents[0] = nil;
        cents[1].visible = not cents[1].visible;

        add_unit(ai[0].controller, math.random() * ScreenWidth(), math.random() * ScreenHeight(), 0xFFFFFFFF);
        clock = game_timer;
    end
end