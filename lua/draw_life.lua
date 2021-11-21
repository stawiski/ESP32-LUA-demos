local drawLife = {}

function drawLife.generation(drawCellFunc, cells)
    for x = 0, #cells do
        for y = 0, #cells[x] do
            drawCellFunc(x, y, cells[x][y] and 1 or 0)
        end
    end
end

function drawLife.generationDiff(drawCellFunc, cellsOld, cellsNew)
    for x = 0, #cellsOld do
        for y = 0, #cellsOld[x] do
            if cellsOld[x][y] ~= cellsNew[x][y] then
                drawCellFunc(x, y, cellsNew[x][y] and 1 or 0)
            end
        end
    end
end

return drawLife
