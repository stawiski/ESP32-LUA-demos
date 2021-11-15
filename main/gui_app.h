#pragma once

// LVGL
#include <lvgl.h>
#include <lvgl_helpers.h>

#define GUI_TASK_STACK 8192

#ifdef __cplusplus
extern "C" {
#endif

void GuiTask(void *pvParameter);
void GuiDrawPixel(uint32_t x, uint32_t y, lv_color_t color);
void GuiDrawSquare(uint32_t x, uint32_t y, uint32_t width, lv_color_t color);

#ifdef __cplusplus
}
#endif
