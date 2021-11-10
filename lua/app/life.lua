local cell = require "app.cell"
local utils = require "app.utils"

function printMap(map)
    print("--- printMap ---")
    for y = 0, #map[0] do
        for x = 0, #map do
            io.write(tostring(map[x][y]))
        end
        io.write('\n')
    end
end

function calculateNewGeneration(map)
    mapNew = {}
    for x = 0, #map do
        mapNew[x] = {}
        for y = 0, #map[x] do
            local cell = map[x][y]
            local liveNeighborsCount =
                countLiveNeightbour(map, x - 1, y - 1) +
                countLiveNeightbour(map, x, y - 1) +
                countLiveNeightbour(map, x + 1, y - 1) +
                countLiveNeightbour(map, x - 1, y) +
                countLiveNeightbour(map, x + 1, y) +
                countLiveNeightbour(map, x - 1, y + 1) +
                countLiveNeightbour(map, x, y + 1) +
                countLiveNeightbour(map, x + 1, y + 1)

            local newCell = utils.clone(cell)
            if cell.isAlive and (liveNeighborsCount < 2 or liveNeighborsCount > 3) then
                newCell.isAlive = false
            end

            if not cell.isAlive and liveNeighborsCount == 3 then
                newCell.isAlive = true
            end

            mapNew[x][y] = newCell
        end
    end

    return mapNew
end

function getNeighbour(map, x, y)
    if x < 0 or x > #map or y < 0 or y > #map[x] then
        return nil
    end
    return map[x][y]
end

function countLiveNeightbour(map, x, y)
    local neighbour = getNeighbour(map, x, y)
    if neighbour == nil then
        return 0
    end
    return neighbour.isAlive and 1 or 0
end

function generateInitialCells(map, mapSizeX, mapSizeY)
    for x = 0, mapSizeX - 1 do
        map[x] = {}
        for y = 0, mapSizeY - 1 do
            local cell = Cell.new(x, y, math.random(0, 10) == 1)
            map[x][y] = cell
        end
    end

    return map
end
