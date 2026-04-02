#pragma once

#include <stdint.h>

struct CatAnimAsset {
  const char* name;
  uint16_t width;
  uint16_t height;
  uint16_t frameCount;
  const uint16_t* const* frames;
};
