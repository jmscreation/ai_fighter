print("AI Loading...");

dir = {};
dir[0] = "l";
dir[1] = "r";
dir[2] = "u";
dir[3] = "d";


function check_surroundings(self)
    data = {};
    data["right"] = check_pos(self.position.x + 32, self.position.y);
    data["up"] = check_pos(self.position.x, self.position.y - 32);
    data["left"] = check_pos(self.position.x - 32, self.position.y);
    data["down"] = check_pos(self.position.x, self.position.y + 32);
    return data;
end


game_timer = 0;
clock = 0;
function unit_loop(self, delta)
    game_timer = game_timer + delta;
    self = get_data(self);

    if (math.random() * 100) > 99 then
        move(self.entity, dir[math.floor(math.random() * 4)]);
    end
    if game_timer - clock > 0.5 then
        local col = check_surroundings(self);
        if col["right"] ~= nil and col["left"] == nil then
            move(self.entity, "left");
        end
        if col["left"] ~= nil and col["right"] == nil then
            move(self.entity, "right");
        end
        if col["up"] ~= nil and col["down"] == nil then
            move(self.entity, "down");
        end
        if col["down"] ~= nil and col["up"] == nil then
            move(self.entity, "up");
        end
        clock = game_timer;
    end
    
end