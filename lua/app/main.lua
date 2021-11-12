
while true do
    for x = 0, 10 do
        draw.drawCell(math.random(0, 49), math.random(0, 49), 1)
    end

    rtos.delayMs(250)
end
