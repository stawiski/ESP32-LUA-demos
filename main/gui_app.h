#pragma once

// LVGL
#include <lvgl.h>
#include <lvgl_helpers.h>

#define GUI_TASK_STACK 8192

#ifdef __cplusplus
extern "C" {
#endif

void GuiInit(void);
void GuiTask(void *pvParameter);
bool GuiDrawSquare(uint16_t x, uint16_t y, uint16_t width, lv_color_t color);

#ifdef __cplusplus
}
#endif
