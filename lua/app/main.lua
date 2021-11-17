-- local life = require "life"

-- map = {}
-- generateInitialCells(map, 15, 15)

-- function drawGeneration(map)
--     for x = 0, #map do
--         for y = 0, #map[x] do
--             local isAlive = 0
--             if map[x][y].isAlive then
--                 isAlive = 1
--             end
--             draw.drawCell(x, y, isAlive)
--         end
--     end
-- end

while true do
    -- drawGeneration(map)
    -- map = calculateNewGeneration(map)
    -- cell.set

    local x = math.random(0, 4)
    local y = math.random(0, 4)
    local isAlive = math.random(0, 1)

    print("setting cell [" .. x .. "," .. y .. "] to " .. isAlive)
    cell.set(math.random(0, 4), math.random(0, 4), math.random(0, 1))

    rtos.delayMs(1000)
end
