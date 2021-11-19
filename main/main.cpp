// STL
#include <stdio.h>
#include <stdlib.h>
// ESP32
#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_spiffs.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// Application
#include "gui_app.h"
#include "lua_app.h"

static void systemTask(void *pvParameter)
{
    (void) pvParameter;

    while (true)
    {
        printf("[SYSTEM] Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

extern "C" void app_main(void)
{
    // Print chip information
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    GuiInit();

    xTaskCreatePinnedToCore(&systemTask, "system", 2 * 1024U, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(&LuaTask, "lua", 16 * 1024U, NULL, 5, NULL, 0);

    // If you want to use a task to create the graphic, you NEED to create a Pinned task
    // Otherwise there can be problem such as memory corruption and so on.
    // NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0
    xTaskCreatePinnedToCore(&GuiTask, "gui", GUI_TASK_STACK, NULL, 0, NULL, 1);
}
