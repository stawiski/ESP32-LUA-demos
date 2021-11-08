// STL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// ESP32
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// Local
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <esp_spiffs.h>
#include <esp_err.h>
#include <esp_log.h>

static const char *TAG = "LUA-VFS";
static const uint32_t LUA_MAX_MEMSIZE = 16 * 1024 * 1024;

static void report(lua_State *L, int status)
{
    if (status == LUA_OK)
    {
        return;
    }

    const char *msg = lua_tostring(L, -1);
    printf("%s\n", msg);
    lua_pop(L, 1);
}

static void halt()
{
    ESP_LOGE(TAG, "System halted");
    while (1)
    {
        vTaskDelay(1000);
    }
}

static void mount_fs()
{
    esp_vfs_spiffs_conf_t conf =
    {
        .base_path = "/lua",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    switch (ret)
    {
        case ESP_OK:
            break;
        case ESP_FAIL:
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
            halt();
            break;
        case ESP_ERR_NOT_FOUND:
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
            halt();
            break;
        default:
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
            halt();
    }
    size_t total = 0, used = 0;
    ESP_ERROR_CHECK(esp_spiffs_info(NULL, &total, &used));
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
}

static size_t lua_mem_size = 0;

static void* l_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
    // LUA is reallocating a memory block, this means it could be:
    // - expanding existing memory block
    // - shrinking existing memory block
    // - allocating new memory block
    // - freeing existing memory block

    if (ptr == NULL)
    {
        // Allocating new object
        lua_mem_size += nsize;
        return malloc(nsize);
    }
    else if (nsize == 0)
    {
        // Freeing existing object
        lua_mem_size -= osize;
        free(ptr);
        return NULL;
    }
    else
    {
        // Reallocating existing object
        size_t new_size = lua_mem_size - osize + nsize;
        if (new_size > LUA_MAX_MEMSIZE)
        {
            printf("Error! Lua wants more memory than we can allocate: %u > %u\n", new_size, LUA_MAX_MEMSIZE);
            return NULL;
        }
        lua_mem_size += nsize - osize;
        return realloc(ptr, nsize);
    }
}

void test(void *arg)
{
    mount_fs();

    while (1)
    {
        lua_State *L = lua_newstate(l_alloc, NULL);
        ESP_ERROR_CHECK(L ? ESP_OK : ESP_FAIL);
        luaL_openlibs(L);
        int r = luaL_loadfilex(L, "/lua/main.lua", NULL);
        if (r != LUA_OK)
        {
            printf("Failed to execute /lua/main.lua\n");
        }
        else
        {
            r = lua_pcall(L, 0, LUA_MULTRET, 0);
        }

        report(L, r);
        lua_close(L);

        printf("[LUA] heap: %d\n", xPortGetFreeHeapSize());
        printf("[LUA] StackHWM: %d\n", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void systemTask(void *pvParameter)
{
    (void) pvParameter;

    while (true)
    {
        printf("[SYSTEM] Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
        printf("[SYSTEM] StackHWM: %d\n", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
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

    xTaskCreate(&systemTask, "system", 2 * 1024, NULL, 1, NULL);
    xTaskCreate(test, "test", 16 * 1024, NULL, 5, NULL);
}
