// FreeRTOS and ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// Local
#include "life_app.h"
#include "gui_app.h"

void LifeTask(void *pvParameter)
{
    (void) pvParameter;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
