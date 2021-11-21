local life = {}

function life.printCells(cells)
    print("--- printCells ---")
    for x = 0, #cells do
        for y = 0, #cells[0] do
            if cells[x][y] then
                io.write("x")
            else
                io.write("o")
            end
        end
        io.write('\n')
    end
end

function getNeighbour(cells, x, y)
    if x < 0 or x > #cells or y < 0 or y > #cells[x] then
        return nil
    end
    return cells[x][y]
end

function countLiveNeightbour(cells, x, y)
    local neighbour = getNeighbour(cells, x, y)
    if neighbour == nil then
        return 0
    end
    return neighbour and 1 or 0
end

function life.nextGeneration(cells)
    cellsNew = {}
    for x = 0, #cells do
        cellsNew[x] = {}
        for y = 0, #cells[x] do
            local liveNeighborsCount =
                countLiveNeightbour(cells, x - 1, y - 1) +
                countLiveNeightbour(cells, x, y - 1) +
                countLiveNeightbour(cells, x + 1, y - 1) +
                countLiveNeightbour(cells, x - 1, y) +
                countLiveNeightbour(cells, x + 1, y) +
                countLiveNeightbour(cells, x - 1, y + 1) +
                countLiveNeightbour(cells, x, y + 1) +
                countLiveNeightbour(cells, x + 1, y + 1)

            -- Make new cell state by copying old cell state
            local isAlive = cells[x][y]

            if isAlive and (liveNeighborsCount < 2 or liveNeighborsCount > 3) then
                isAlive = false
            elseif not isAlive and liveNeighborsCount == 3 then
                isAlive = true
            end

            cellsNew[x][y] = isAlive
        end
    end

    return cellsNew
end

function life.randomizeGeneration(cells, width, height)
    for x = 0, width - 1 do
        cells[x] = {}
        for y = 0, height - 1 do
            cells[x][y] = sys.randf() < 0.3
        end
    end

    return cells
end

return life
