local life = require "life"
local drawLife = require "draw_life"

CELL_SIZE = 10

cellsCurrent = {}
life.randomizeGeneration(cellsCurrent, 32, 24)

function drawCell(x, y, isAlive)
    draw.square(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, isAlive)
end

-- drawLife.generation(drawCell, cellsCurrent)

while true do
    -- print("next gen")
    drawLife.generation(drawCell, cellsCurrent)
    cellsCurrent = life.nextGeneration(cellsCurrent)
    -- local cellsNext = life.nextGeneration(cellsCurrent)
    -- print("draw diff")
    -- drawLife.generationDiff(drawCell, cellsCurrent, cellsNext)
    -- cellsCurrent = cellsNext
    sys.delayMs(1000)
end
