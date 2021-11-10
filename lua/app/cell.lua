Cell = {}

function Cell.new(x, y, isAlive)
    local o = {}
    o.x = x
    o.y = y
    o.isAlive = isAlive
    local meta = {}
    meta.__index = Cell
    meta.__tostring = function (obj)
        if obj.isAlive then
            return "x"
        else
            return "o"
        end
    end
    setmetatable(o, meta)
    return o
end
