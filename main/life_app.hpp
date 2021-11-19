#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void LifeInit(void);
void LifeTask(void *pvParameter);
bool LifeCellGet(uint32_t x, uint32_t y);
bool LifeCellSet(uint32_t x, uint32_t y, bool isAlive);
bool LifeCellCommit(void);

#ifdef __cplusplus
}
#endif
