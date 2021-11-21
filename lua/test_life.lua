local life = require "life"
local drawLife = require "draw_life"

-- Provide sys object
_G.sys = {}
_G.sys['randf'] = math.random

local drawCell = function(x, y, isAlive)
    print("drawSquare (" .. x .. ", " .. y .. ") " .. ((isAlive == 1) and "x" or "o"))
end

describe("Life", function()
    describe("Generation of new cells", function()
        cells = {}
        cells[0] = {}
        it("One cell on a 1x1 grid", function()
            cells[0][0] = true
            local cellsNext = life.nextGeneration(cells)
            assert.truthy(cellsNext[0][0] == false)
        end)
        it("Three horizontal cells on a small 3x3 grid", function()
            cells[0][0] = false
            cells[0][1] = true
            cells[0][2] = false
            cells[1] = {}
            cells[1][0] = false
            cells[1][1] = true
            cells[1][2] = false
            cells[2] = {}
            cells[2][0] = false
            cells[2][1] = true
            cells[2][2] = false
            print(" --- before ---")
            life.printCells(cells)
            local cellsNext = life.nextGeneration(cells)
            print(" --- after ---")
            life.printCells(cellsNext)
            print("draw cells gen =====")
            drawLife.generation(drawCell, cells)
            print("draw cells next gen =====")
            drawLife.generation(drawCell, cellsNext)
            print("draw cells diff gen =====")
            drawLife.generationDiff(drawCell, cells, cellsNext)
            cells = cellsNext
            print("draw cells again")
            drawLife.generation(drawCell, cells)
            -- Two columns have no cells
            assert.truthy(cellsNext[0][0] == false)
            assert.truthy(cellsNext[0][1] == false)
            assert.truthy(cellsNext[0][2] == false)
            assert.truthy(cellsNext[2][0] == false)
            assert.truthy(cellsNext[2][1] == false)
            assert.truthy(cellsNext[2][2] == false)
            -- Middle colum has alive cells
            assert.truthy(cellsNext[1][0] == true)
            assert.truthy(cellsNext[1][1] == true)
            assert.truthy(cellsNext[1][2] == true)
        end)
        it("Generating random 5x5 grid", function()
            local cells = life.randomizeGeneration(cells, 5, 5)
            life.printCells(cells)
        end)
    end)
end)
