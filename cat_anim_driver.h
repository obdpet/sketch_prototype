#pragma once

#include <stdint.h>
#include "cat_anim_registry.h"

extern int g_catCharacterIndex;
extern int g_catAnimObdKmh;

inline int catAnimCountLoadedSlots(const CatCharacterPack& pack) {
  int n = 0;
  for (int i = 0; i < CAT_ANIM_SLOT_COUNT; ++i) {
    if (pack.slot[i]) ++n;
  }
  return n;
}

inline const CatAnimAsset* catAnimFirstNonNullSlot(const CatCharacterPack& pack) {
  for (int i = 0; i < CAT_ANIM_SLOT_COUNT; ++i) {
    if (pack.slot[i]) return pack.slot[i];
  }
  return nullptr;
}

/**
 * OBD 속도(km/h 등)로 재생할 클립 선택.
 * - 로드된 슬롯이 1개뿐이면 상태와 무관하게 그 클립만 반복.
 * - 2개 이상: obdKmh<=0 → SLEEP 우선, 그다음 IDLE, RUN … / obdKmh>0 → RUN 우선, 그다음 IDLE, SLEEP …
 */
inline const CatAnimAsset* catAnimResolvePlayingAsset(const CatCharacterPack& pack, int obdKmh) {
  const int n = catAnimCountLoadedSlots(pack);
  if (n == 0) return nullptr;
  if (n == 1) return catAnimFirstNonNullSlot(pack);

  const CatAnimAsset* sleep = pack.slot[CAT_ANIM_SLEEP];
  const CatAnimAsset* run = pack.slot[CAT_ANIM_RUN];
  const CatAnimAsset* idle = pack.slot[CAT_ANIM_IDLE];
  const CatAnimAsset* err = pack.slot[CAT_ANIM_ERROR];

  if (obdKmh <= 0) {
    if (sleep) return sleep;
    if (idle) return idle;
    if (run) return run;
    if (err) return err;
    return catAnimFirstNonNullSlot(pack);
  }
  if (run) return run;
  if (idle) return idle;
  if (sleep) return sleep;
  if (err) return err;
  return catAnimFirstNonNullSlot(pack);
}

inline const CatAnimAsset* catAnimPeekPlayingAsset() {
  if (g_catCharacterIndex < 0 || g_catCharacterIndex >= kCatCharacterCount) return nullptr;
  return catAnimResolvePlayingAsset(kCatCharacters[g_catCharacterIndex], g_catAnimObdKmh);
}
