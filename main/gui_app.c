// STL
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// FreeRTOS and ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_freertos_hooks.h>
#include <freertos/semphr.h>
#include <esp_system.h>
// Local
#include "gui_app.h"
#include "utils.h"

#define LV_TICK_PERIOD_MS 1

// Forward declarations

static void lv_tick_task(void *arg);

// Private variables

static SemaphoreHandle_t xGuiSemaphore;

static EXT_RAM_ATTR lv_color_t  ssFrameBufferA[DISP_BUF_SIZE * sizeof(lv_color_t)],
                                ssFrameBufferB[DISP_BUF_SIZE * sizeof(lv_color_t)];

static EXT_RAM_ATTR lv_color_t ssCanvasBuffer[LCD_WIDTH_PX * LCD_HEIGHT_PX * sizeof(lv_color_t)];
static lv_obj_t *ssCanvasPtr;

// Implementation

// Private functions

static void create_demo_app(void)
{
    ssCanvasPtr = lv_canvas_create(lv_scr_act(), NULL);
    lv_canvas_set_buffer(ssCanvasPtr, ssCanvasBuffer, LCD_WIDTH_PX, LCD_HEIGHT_PX, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(ssCanvasPtr, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_fill_bg(ssCanvasPtr, LV_COLOR_GREEN, LV_OPA_COVER);
    lv_obj_invalidate(ssCanvasPtr);
}

static void lv_tick_task(void *arg)
{
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

// Public functions

void GuiTask(void *pvParameter)
{
    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    lv_init();
    lvgl_driver_init();

    static lv_disp_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820         \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A    \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D     \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, ssFrameBufferA, ssFrameBufferB, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    // Create demo
    create_demo_app();

    // Give the semaphore as the GUI task is ready
    xSemaphoreGive(xGuiSemaphore);

    uint32_t guiTaskLoop = 0;
    uint32_t lastLogPrintTimeMs = 0;

    while (1)
    {
        if (millis() - lastLogPrintTimeMs >= 1000)
        {
            printf("[GUI] Loop: %d\n", guiTaskLoop);
            printf("[GUI] StackHWM: %d\n", uxTaskGetStackHighWaterMark(NULL));
            lastLogPrintTimeMs = millis();
        }

        // Try to take the semaphore, call lvgl related function on success
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }

        guiTaskLoop++;
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    printf("[GUI] Task ended\n");

    // A task should NEVER return.
    vTaskDelete(NULL);
}

void GuiDrawPixel(uint32_t x, uint32_t y, lv_color_t color)
{
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    {
        lv_canvas_set_px(ssCanvasPtr, x, y, color);
        lv_obj_invalidate(ssCanvasPtr);
        xSemaphoreGive(xGuiSemaphore);
    }
}

void GuiDrawSquare(uint32_t x, uint32_t y, uint32_t width, lv_color_t color)
{
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    {
        lv_draw_rect_dsc_t rectangle;
        lv_draw_rect_dsc_init(&rectangle);
        rectangle.bg_color = color;
        lv_canvas_draw_rect(ssCanvasPtr, x, y, width, width, &rectangle);
        xSemaphoreGive(xGuiSemaphore);
    }
}
