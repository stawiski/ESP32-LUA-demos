// STL
#include <algorithm>
// FreeRTOS and ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
// Local
#include "life_app.hpp"
#include "gui_app.h"
#include "utils.h"

// Definitions

static const uint32_t CELL_SIZE = 10;
static const uint32_t CELL_MAP_WIDTH = LCD_WIDTH_PX / CELL_SIZE;
static const uint32_t CELL_MAP_HEIGHT = LCD_HEIGHT_PX / CELL_SIZE;

typedef struct cellArray
{
    bool cells[CELL_MAP_WIDTH][CELL_MAP_HEIGHT];

} cellArray_t;

// Private variables

static uint32_t ssDrawFailedCounter;
static cellArray_t ssCellMapDrawn, ssCellMap;
static SemaphoreHandle_t xLifeSemaphore;

static void clearCellMap(cellArray_t &cellMap)
{
    for (uint32_t x = 0; x < CELL_MAP_WIDTH; x++)
    {
        for (uint32_t y = 0; y < CELL_MAP_HEIGHT; y++)
        {
            cellMap.cells[x][y] = false;
        }
    }
}

static void randomizeCellMap(cellArray_t &cellMap)
{
    for (uint32_t x = 0; x < CELL_MAP_WIDTH; x++)
    {
        for (uint32_t y = 0; y < CELL_MAP_HEIGHT; y++)
        {
            cellMap.cells[x][y] = (randf() <= 0.1f);
        }
    }
}

bool operator==(const cellArray_t &lhs, const cellArray_t &rhs)
{
    for (uint32_t x = 0; x < CELL_MAP_WIDTH; x++)
    {
        for (uint32_t y = 0; y < CELL_MAP_HEIGHT; y++)
        {
            if (lhs.cells[x][y] != rhs.cells[x][y])
            {
                return false;
            }
        }
    }

    return true;
}

bool operator!=(const cellArray_t &lhs, const cellArray_t &rhs)
{
    return !(lhs == rhs);
}

static void updateCells(void)
{
    // Go through the cell map to compare new and drawn cells
    for (uint32_t x = 0; x < CELL_MAP_WIDTH; x++)
    {
        for (uint32_t y = 0; y < CELL_MAP_HEIGHT; y++)
        {
            if (ssCellMapDrawn.cells[x][y] != ssCellMap.cells[x][y])
            {
                // Update cells that have changed
                if (!GuiDrawSquare(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, ssCellMap.cells[x][y] ? LV_COLOR_BLACK : LV_COLOR_WHITE))
                {
                    ssDrawFailedCounter++;
                }
                ssCellMapDrawn.cells[x][y] = ssCellMap.cells[x][y];
            }
        }
    }
}

void LifeInit(void)
{
    ssDrawFailedCounter = 0;
    clearCellMap(ssCellMapDrawn);
    clearCellMap(ssCellMap);
    xLifeSemaphore = xSemaphoreCreateBinary();
}

void LifeTask(void *pvParameter)
{
    (void) pvParameter;

    xSemaphoreGive(xLifeSemaphore);
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {
        // randomizeCellMap(ssCellMap);

        if (xSemaphoreTake(xLifeSemaphore, portMAX_DELAY) == pdTRUE)
        {
            updateCells();
            xSemaphoreGive(xLifeSemaphore);
        }
        printf("[LIFE] StackHWM: %d\n", uxTaskGetStackHighWaterMark(NULL));
        printf("[LIFE] ssDrawFailedCounter = %d\n", ssDrawFailedCounter);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    vTaskDelete(NULL);
}

bool LifeCellGet(uint32_t x, uint32_t y)
{
    return ssCellMap.cells[x][y];
}

bool LifeCellSet(uint32_t x, uint32_t y, bool isAlive)
{
    if (x >= CELL_MAP_WIDTH || y >= CELL_MAP_HEIGHT)
    {
        return false;
    }

    if (xSemaphoreTake(xLifeSemaphore, portMAX_DELAY) == pdTRUE)
    {
        ssCellMap.cells[x][y] = isAlive;
        xSemaphoreGive(xLifeSemaphore);
        return true;
    }

    return false;
}
