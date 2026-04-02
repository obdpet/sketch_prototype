#pragma once

#include <stdint.h>

// sketch_prototype.ino의 setup()에서 tft 회전 반영 후 설정
extern uint16_t g_catSprW;
extern uint16_t g_catSprH;

#define CAT_CX (g_catSprW / 2)
#define CAT_CY (g_catSprH / 2)
