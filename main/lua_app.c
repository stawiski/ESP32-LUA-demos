// STL
#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <math.h>
// ESP32
#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_spiffs.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// Components
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
#include <lwmem/lwmem.h>
// Local
#include "gui_app.h"

#ifndef LUA_HEAP_SIZE
    #error "LUA_HEAP_SIZE not defined"
#endif

uint8_t EXT_RAM_ATTR ssLuaHeapRegion[LUA_HEAP_SIZE];
static size_t ssLuaUsedHeap = 0, ssLuaMaxHeapUsed = 0;

static lwmem_region_t ssLwmemHeapRegions[] =
{
    {ssLuaHeapRegion, sizeof(ssLuaHeapRegion)},
};

static const lwmem_region_t* getLwmemHeapRegionForLua(void)
{
    return &ssLwmemHeapRegions[0];
}

static const char *TAG = "LUA-VFS";

static void halt()
{
    ESP_LOGE(TAG, "System halted");
    while (1)
    {
        vTaskDelay(1000);
    }
}

static void mount_fs(void)
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

static void* luaMemoryAllocator(void *ud, void *ptr, size_t osize, size_t nsize)
{
    // LUA is reallocating a memory block, this means it could be:
    // - expanding existing memory block
    // - shrinking existing memory block
    // - allocating new memory block
    // - freeing existing memory block

    if (ptr == NULL)
    {
        // Allocating new object
        ssLuaUsedHeap += nsize;
        ssLuaMaxHeapUsed = (ssLuaUsedHeap > ssLuaMaxHeapUsed) ? ssLuaUsedHeap : ssLuaMaxHeapUsed;
        return lwmem_malloc_ex(NULL, getLwmemHeapRegionForLua(), nsize);
    }
    else if (nsize == 0)
    {
        // Freeing existing object
        ssLuaUsedHeap -= osize;
        lwmem_free_ex(NULL, ptr);
        return NULL;
    }
    else
    {
        // Reallocating existing object
        size_t new_size = ssLuaUsedHeap - osize + nsize;
        if (new_size > LUA_HEAP_SIZE)
        {
            printf("Error! Lua wants more memory than we can allocate: %u > %u\n", new_size, LUA_HEAP_SIZE);
            return NULL;
        }
        ssLuaUsedHeap += nsize - osize;
        ssLuaMaxHeapUsed = (ssLuaUsedHeap > ssLuaMaxHeapUsed) ? ssLuaUsedHeap : ssLuaMaxHeapUsed;
        return lwmem_realloc_ex(NULL, getLwmemHeapRegionForLua(), ptr, nsize);
    }
}

float calculateFreeHeapPrecentage(uint32_t heapUsedBytes, uint32_t heapSizeBytes)
{
    return 100.0f - ((((float)heapUsedBytes) * 100.0f) / ((float)heapSizeBytes));
}

static int luaDrawCell(lua_State *L)
{
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int isAlive = luaL_checkinteger(L, 3);
    GuiDrawPixel(x, y, isAlive ? LV_COLOR_BLACK : LV_COLOR_WHITE);
    return 0;
}

static int luaDelayMs(lua_State *L)
{
    int ms = luaL_checkinteger(L, 1);
    vTaskDelay(pdMS_TO_TICKS(ms));
    return 0;
}

static const struct luaL_Reg ssLuaDrawFuncs[] =
{
    {"drawCell", luaDrawCell},
    {NULL, NULL}
};

static const struct luaL_Reg ssLuaRtosFuncs[] =
{
    {"delayMs", luaDelayMs},
    {NULL, NULL}
};

static int luaopen_lDraw(lua_State *L)
{
    luaL_newlib(L, ssLuaDrawFuncs);
    return 1;
}

static int luaopen_lRtos(lua_State *L)
{
    luaL_newlib(L, ssLuaRtosFuncs);
    return 1;
}

static void luaLoadCustomLibs(lua_State *lua)
{
    luaL_requiref(lua, "draw", luaopen_lDraw, 1);
    lua_pop(lua, 1);
    luaL_requiref(lua, "rtos", luaopen_lRtos, 1);
    lua_pop(lua, 1);
}

static lua_State *lua;

void LuaTask(void *arg)
{
    mount_fs();
    assert(lwmem_assignmem(ssLwmemHeapRegions, LWMEM_ARRAYSIZE(ssLwmemHeapRegions)));

    lua = lua_newstate(luaMemoryAllocator, NULL);
    ESP_ERROR_CHECK(lua ? ESP_OK : ESP_FAIL);

    luaL_openlibs(lua);
    luaLoadCustomLibs(lua);

    int r = luaL_loadfilex(lua, "/lua/main.lua", NULL);
    ESP_ERROR_CHECK(r == LUA_OK ? ESP_OK : ESP_FAIL);

    // while (1)
    {
        // lua_newthread

        if (lua_pcall(lua, 0, 0, 0) != LUA_OK)
        {
            const char *msg = lua_tostring(lua, -1);
            printf("%s\n", msg);
            lua_pop(lua, 1);
            // break;
        }

        printf("[LUA] Maximum heap usage %u/%u (%.1f%% free)\n", ssLuaMaxHeapUsed, LUA_HEAP_SIZE, calculateFreeHeapPrecentage(ssLuaMaxHeapUsed, LUA_HEAP_SIZE));
        printf("[LUA] Task heap: %d\n", xPortGetFreeHeapSize());
        printf("[LUA] Stack HWM: %d\n", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    printf("[LUA] Task ended\n");
    lua_close(lua);
    vTaskDelete(NULL);
}
