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
function setup()
    Entity.new(48, 48, Canvas.new(48,48,0xFFF00FFF));
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

    
    if game_timer - clock > 1 then
        Entity.new(math.random() * ScreenWidth(), math.random() * ScreenHeight(), Canvas.new(48,48,0xFFF00FFF));
        clock = game_timer;
    end
end