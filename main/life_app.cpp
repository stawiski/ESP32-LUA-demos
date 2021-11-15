// FreeRTOS and ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// Local
#include "life_app.hpp"
#include "gui_app.h"

static const uint32_t CELL_SIZE = 10;
static const uint32_t CELL_MAP_WIDTH = LCD_WIDTH_PX / CELL_SIZE;
static const uint32_t CELL_MAP_HEIGHT = LCD_HEIGHT_PX / CELL_SIZE;

static bool ssCellMap[CELL_MAP_WIDTH][CELL_MAP_HEIGHT];

void LifeTask(void *pvParameter)
{
    (void) pvParameter;

    for (auto& cellRow : ssCellMap)
    {
        for (auto& cell : cellRow)
        {
            cell = false;
        }
    }

    while (1)
    {
        printf("[LIFE] is life!\n");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
