if SERVER then
    return
end

require("zenai")

local color_red = Color(255, 0, 0)
local color_green = Color(0, 255, 0)
local color_blue = Color(0, 0, 255)
local color_purple = Color(255, 0, 255)
local mins, maxs = Vector(-24, -3, -2), Vector(24, 3, 2)
    
local view_grid_distance_table = {}
local view_grid_classname_table = {}

local view_grid_size_x = 16
local view_grid_size_y = 16
local view_grid_scale = 100
local view_grid_max_distance = 1000
local last_damage_direction = 0
local last_damage = 0
local last_reward = 0

local combine_npc_table = {
    ["npc_combine_s"] = true,
    ["npc_helicopter"] = true,
    ["npc_combinedropship"] = true,
    ["npc_combinegunship"] = true,
    ["npc_strider"] = true,
    ["npc_metropolice"] = true,
    ["npc_rollermine"] = true,
    ["npc_turret_ceiling"] = true,
    ["npc_turret_floor"] = true
}

hook.Add("EntityTakeDamage", "zenai_received_damage_direction", function(target, dmg)

    if target == LocalPlayer() then
        local attacker = dmg:GetAttacker()
        local direction = (target:GetPos() - attacker:GetPos()):GetNormalized()

        local direction_compass = ""
        if direction.x > 0.5 then
            direction_compass = "E"
        elseif direction.x < -0.5 then
            direction_compass = "W"
        end
        if direction.y > 0.5 then
            direction_compass = "N" .. direction_compass
        elseif direction.y < -0.5 then
            direction_compass = "S" .. direction_compass
        end

        local direction_normalized = 0
        if direction_compass == "E" then
            direction_normalized = 0.125
        elseif direction_compass == "NE" then
            direction_normalized = 0.250
        elseif direction_compass == "N" then
            direction_normalized = 0.375
        elseif direction_compass == "NW" then
            direction_normalized = 0.500
        elseif direction_compass == "W" then
            direction_normalized = 0.625
        elseif direction_compass == "SW" then
            direction_normalized = 0.750
        elseif direction_compass == "S" then
            direction_normalized = 0.875
        elseif direction_compass == "SE" then
            direction_normalized = 1.000
        end

        last_damage_direction = direction_normalized
        last_damage = dmg:GetDamage()
    end

    if combine_npc_table[target:GetClass()] ~= nil then
        last_reward = dmg:GetDamage() * 0.2
    end
end)

hook.Add("Think", "zenai_think", function()
    if #view_grid_distance_table == 0 or #view_grid_classname_table == 0 then
        return
    end

    local player_status = 0
    if LocalPlayer():OnGround() == false then
        player_status = 0.2
    elseif LocalPlayer():WaterLevel() > 0 then
        player_status = 0.4
    elseif LocalPlayer():InVehicle() then
        if LocalPlayer():GetVehicle():GetClass() == "prop_vehicle_jeep" then
            player_status = 0.6
        elseif LocalPlayer():GetVehicle():GetClass() == "prop_vehicle_airboat" then
            player_status = 0.8
        end
    end

    local game_state_table = {
        ["map_name"] = game.GetMap(),
        ["time_spent"] = CurTime(),
        ["player_status"] = player_status,
        ["equipped_weapon"] = LocalPlayer():GetActiveWeapon():GetClass(),
        ["incoming_damage_direction"] = last_damage_direction,
        ["incoming_damage_amount"] = last_damage,
        ["reward"] = last_reward,
    }

    if last_damage_direction ~= 0 or last_damage ~= 0 then
        last_damage_direction = 0
        last_damage = 0
    end
    if last_reward ~= 0 then
        last_reward = 0
    end

    RegisterGameState(game_state_table, view_grid_distance_table, view_grid_classname_table)
end)

hook.Add("PostDrawTranslucentRenderables", "zenai_trace_visualize", function()
    local eyePos = Entity(1):EyePos() + Entity(1):GetRight() * -5
    local eyeDir = Entity(1):GetAimVector()

    local index = 1

    for x = 1, view_grid_size_x do
        for y = 1, view_grid_size_y do
            local tr = util.TraceLine({
                start = eyePos,
                endpos = eyePos + eyeDir * view_grid_max_distance + LocalPlayer():GetRight() *
                    (x - view_grid_size_x / 2) * view_grid_scale + LocalPlayer():GetUp() * (y - view_grid_size_y / 2) *
                    view_grid_scale,
                filter = Entity(1)
            })

            view_grid_distance_table[index] = 1 - tr.Fraction
            local color = color_green

            if tr.Hit then
                view_grid_classname_table[index] = tr.Entity:GetClass()
                if tr.Entity:IsWorld() then
                    color = color_blue
                else
                    color = color_purple
                end
            else
                view_grid_classname_table[index] = ""
            end

            render.DrawLine(eyePos, tr.HitPos, color, true)

            index = index + 1
        end
    end
end)
