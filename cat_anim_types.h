#pragma once

#include <stdint.h>

struct CatAnimAsset {
  const char* name;
  uint16_t width;
  uint16_t height;
  uint16_t frameCount;
  const uint16_t* const* frames;
};

/** 슬롯 순서가 해석 우선순위에 사용됩니다. 새 상태는 끝에 추가하세요. */
enum CatAnimSlot : uint8_t {
  CAT_ANIM_SLEEP = 0,
  CAT_ANIM_RUN = 1,
  CAT_ANIM_IDLE = 2,
  CAT_ANIM_ERROR = 3,
  CAT_ANIM_SLOT_COUNT
};

/** 논리 경로 /animations/{id}/… — 빌드에 포함된 PROGMEM 클립 묶음 */
struct CatCharacterPack {
  const char* id;
  const CatAnimAsset* slot[CAT_ANIM_SLOT_COUNT];
};
