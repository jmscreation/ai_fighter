-- global variables
game_timer = 0;

-- mini lib functions
function sign(x)
    return x>0 and 1 or x<0 and -1 or 0
end
function near(x1,x2)
    return math.abs(x2-x1) < 1;
end


-- Main Game Setup Function
local t = Entity.new(Canvas.new(48,48,0xFFF00FFF));
function setup()
    t.x = 48;
    t.y = 48;
    print("Initialize...\n");

    --ScreenWidth(); -- get screen width
    --ScreenHeight(); -- get screen height
    --ScreenResize(w, h); -- resize internal screen
    --WindowFullscreen(full); -- set fullscreen mode
    --WindowVsync(on); -- Vsync enabled
    --WindowResize(800, 600); -- Change window size
    --find_entity(id); -- find an entity based on its ID value (references lost to entity will no longer be valid)
    --register("name", function(argument)end); -- register a function for any AI script to call
    --list_ai(); -- list all AI scripts that are available and running


end

-- Main Game Loop Function
local clock = 0;
function loop(delta)
    game_timer = game_timer + delta;
    t.x = t.x + delta * (64 * math.random() - 32);
    t.y = t.y + delta * (64 * math.random() - 32);
    
    if game_timer - clock > 1 then
        t = Entity.new(math.random() * ScreenWidth(), math.random() * ScreenHeight(), Canvas.new(48,48,0xFFF00FFF));
        clock = game_timer;
    end
end