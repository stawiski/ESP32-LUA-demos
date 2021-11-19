local life = require "life"

CELL_SIZE = 10
map = {}
generateInitialCells(map, 15, 15)

function drawGeneration(map)
    for x = 0, #map do
        for y = 0, #map[x] do
            draw.square(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, map[x][y] and 1 or 0)
        end
    end
end

while true do
    drawGeneration(map)
    map = calculateNewGeneration(map)
    sys.delayMs(1000)
end
