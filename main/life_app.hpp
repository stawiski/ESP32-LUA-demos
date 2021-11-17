#pragma once

#include <cstdint>
#include <cstdbool>

void LifeInit(void);
void LifeTask(void *pvParameter);
bool LifeCellGet(uint32_t x, uint32_t y);
bool LifeCellSet(uint32_t x, uint32_t y, bool isAlive);
