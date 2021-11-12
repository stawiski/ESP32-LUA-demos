#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Generate random LV color.
///
/// @return Randomized color.
lv_color_t makeRandomColor(void);

/// Get random float in range [0;1].
///
/// @return Random float number.
float randf(void);

/// Get current time in microseconds.
///
/// Arduino-like API.
///
/// @return Current time in microseconds.
uint64_t micros(void);

/// Get current time in milliseconds.
///
/// Arduino-like API.
///
/// @return Current time in milliseconds.
uint64_t millis(void);

#ifdef __cplusplus
}
#endif
