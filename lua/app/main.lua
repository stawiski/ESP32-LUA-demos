local life = require "life"

map = {}
generateInitialCells(map, 15, 15)

function drawGeneration(map)
    for x = 0, #map do
        for y = 0, #map[x] do
            local isAlive = 0
            if map[x][y].isAlive then
                isAlive = 1
            end
            draw.drawCell(x, y, isAlive)
        end
    end
end

while true do
    drawGeneration(map)
    map = calculateNewGeneration(map)
    rtos.delayMs(250)
end
