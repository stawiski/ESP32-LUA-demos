local life = {}

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
            local liveNeighborsCount =
                countLiveNeightbour(map, x - 1, y - 1) +
                countLiveNeightbour(map, x, y - 1) +
                countLiveNeightbour(map, x + 1, y - 1) +
                countLiveNeightbour(map, x - 1, y) +
                countLiveNeightbour(map, x + 1, y) +
                countLiveNeightbour(map, x - 1, y + 1) +
                countLiveNeightbour(map, x, y + 1) +
                countLiveNeightbour(map, x + 1, y + 1)

            -- Make new cell state by copying old cell state
            local isAlive = map[x][y]

            if isAlive and (liveNeighborsCount < 2 or liveNeighborsCount > 3) then
                isAlive = false
            elseif not isAlive and liveNeighborsCount == 3 then
                isAlive = true
            end

            mapNew[x][y] = isAlive
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
    return neighbour and 1 or 0
end

function generateInitialCells(map, mapSizeX, mapSizeY)
    for x = 0, mapSizeX - 1 do
        map[x] = {}
        for y = 0, mapSizeY - 1 do
            map[x][y] = sys.randf() < 0.3
        end
    end

    return map
end

return life
