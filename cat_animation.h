#pragma once

/*
  빌드 시 생성된 RGB565 프레임 헤더를 순차 출력합니다.
*/

#include <TFT_eSPI.h>
#include <math.h>
#include <pgmspace.h>
#include <string.h>
#if CAT_ANIM_USE_PUSH_IMAGE
#include <stdlib.h>
#endif
#include "cat_anim_picker.h"
#include "cat_anim_registry.h"
#include "cat_layout.h"

static const CatAnimAsset* s_currentAnim = nullptr;
static int s_currentFrame = 0;
static bool s_animHasBitmap = false;

#if CAT_ANIM_USE_PUSH_IMAGE
static uint16_t* s_push565Buf = nullptr;
static size_t s_push565Cap = 0;

static bool catAnimEnsurePush565(int sw, int sh) {
  size_t need = (size_t)sw * (size_t)sh;
  if (need == 0) return false;
  if (need > s_push565Cap || s_push565Buf == nullptr) {
    free(s_push565Buf);
    s_push565Buf = (uint16_t*)malloc(need * sizeof(uint16_t));
    s_push565Cap = s_push565Buf ? need : 0;
  }
  return s_push565Buf != nullptr;
}
#endif

#if CAT_ANIM_DIM_PERCENT < 100
static uint16_t catAnimDim565(uint16_t c) {
  unsigned r = (c >> 11) & 31u;
  unsigned g = (c >> 5) & 63u;
  unsigned b = c & 31u;
  unsigned p = (unsigned)CAT_ANIM_DIM_PERCENT;
  r = (r * p) / 100u;
  g = (g * p) / 100u;
  b = (b * p) / 100u;
  return (uint16_t)((r << 11) | (g << 5) | b);
}
#endif

static uint16_t catAnimProcessColor(uint16_t color) {
#if CAT_ANIM_SWAP_RB
  color = (uint16_t)((color & 0x07E0) | ((color & 0xF800) >> 11) | ((color & 0x001F) << 11));
#endif
#if CAT_ANIM_INVERT_COLORS
  color ^= 0xFFFF;
#endif
#if CAT_ANIM_DIM_PERCENT < 100
  color = catAnimDim565(color);
#endif
  return color;
}

static const CatAnimAsset* catAnimLookupCurrent() {
  catAnimEnsureDefaultPath();
  const CatAnimAsset* a = catAnimPeekPlayingAsset();
  if (a && a->name) {
    strncpy(g_animGifPath, a->name, sizeof(g_animGifPath) - 1);
    g_animGifPath[sizeof(g_animGifPath) - 1] = '\0';
  } else {
    g_animGifPath[0] = '\0';
  }
  return a;
}

static uint16_t catAnimAverage565Four(uint16_t c0, uint16_t c1, uint16_t c2, uint16_t c3) {
  unsigned r = ((c0 >> 11) & 31u) + ((c1 >> 11) & 31u) + ((c2 >> 11) & 31u) + ((c3 >> 11) & 31u);
  unsigned g = ((c0 >> 5) & 63u) + ((c1 >> 5) & 63u) + ((c2 >> 5) & 63u) + ((c3 >> 5) & 63u);
  unsigned b = (c0 & 31u) + (c1 & 31u) + (c2 & 31u) + (c3 & 31u);
  r = (r + 2u) / 4u;
  g = (g + 2u) / 4u;
  b = (b + 2u) / 4u;
  return (uint16_t)((r << 11) | (g << 5) | b);
}

/** 네 모서리 평균(디스플레이 보정 포함). fi = 해당 프레임 인덱스. */
inline uint16_t catAnimGetBorderColorForFrame(uint16_t fallback, int frameIndex) {
  const CatAnimAsset* anim = catAnimLookupCurrent();
  if (!anim || anim->width < 1 || anim->height < 1 || anim->frameCount < 1) return fallback;

  int fi = frameIndex;
  if (fi < 0 || fi >= anim->frameCount) fi = 0;

#if defined(__AVR__) || defined(ESP8266)
  const uint16_t* frameData =
      reinterpret_cast<const uint16_t*>(pgm_read_ptr(&anim->frames[fi]));
#else
  const uint16_t* frameData = anim->frames[fi];
#endif
  const int w = anim->width;
  const int h = anim->height;
  uint16_t p00 = catAnimProcessColor(pgm_read_word(&frameData[0]));
  uint16_t p10 = catAnimProcessColor(pgm_read_word(&frameData[(size_t)w - 1]));
  uint16_t p01 = catAnimProcessColor(pgm_read_word(&frameData[(size_t)(h - 1) * (size_t)w]));
  uint16_t p11 = catAnimProcessColor(pgm_read_word(&frameData[(size_t)(h - 1) * (size_t)w + (size_t)w - 1]));
  return catAnimAverage565Four(p00, p10, p01, p11);
}

/** 현재 재생 프레임 기준(동적). */
inline uint16_t catAnimGetBorderColor(uint16_t fallback) {
  return catAnimGetBorderColorForFrame(fallback, s_currentFrame);
}

/** 0번 프레임만 사용 — TFT 여백 채움 등 프레임마다 색이 바뀌며 깜빡이지 않게 할 때. */
inline uint16_t catAnimGetBorderColorStable(uint16_t fallback) {
  return catAnimGetBorderColorForFrame(fallback, 0);
}

static void catAnimDrawFrame(TFT_eSprite& sprite, const CatAnimAsset& anim, int frameIndex, uint16_t bgColor) {
  const int sw = (int)g_catSprW;
  const int sh = (int)g_catSprH;
  if (anim.width < 1 || anim.height < 1 || sw < 1 || sh < 1) return;

  uint16_t fillCol = bgColor;
#if CAT_ANIM_FIT_CONTAIN && CAT_ANIM_LETTERBOX_FROM_ANIM_BORDER
  fillCol = catAnimGetBorderColorStable(bgColor);
#endif

#if CAT_ANIM_USE_PUSH_IMAGE
  const bool usePush = catAnimEnsurePush565(sw, sh);
  if (usePush) {
    size_t n = (size_t)sw * (size_t)sh;
    for (size_t i = 0; i < n; i++) s_push565Buf[i] = fillCol;
  } else
#endif
  {
    sprite.fillSprite(fillCol);
  }

  // 부동소수 스케일로 가로·세로 동일 비율 유지(정수/1000 방식보다 왜곡 적음)
  bool scaleWidthToSprite = false;
#if CAT_ANIM_FIT_CONTAIN
  float factor = fminf((float)sw / (float)anim.width, (float)sh / (float)anim.height);
  {
    const int preW = (int)lroundf((float)anim.width * factor);
    // contain 결과 가로가 스프라이트보다 작으면 가로를 sw에 맞춤(비율 유지, 세로는 레터박스 또는 상하 크롭)
    if (preW < sw) {
      factor = (float)sw / (float)anim.width;
      scaleWidthToSprite = true;
    }
  }
#else
  float factor = fmaxf((float)sw / (float)anim.width, (float)sh / (float)anim.height);
#endif
  if (factor < 1e-6f) factor = 1e-6f;
  int scaledW = (int)lroundf((float)anim.width * factor);
  int scaledH = (int)lroundf((float)anim.height * factor);
  if (scaledW < 1) scaledW = 1;
  if (scaledH < 1) scaledH = 1;
#if CAT_ANIM_FIT_CONTAIN
  if (scaleWidthToSprite) {
    scaledW = sw;
  } else {
    if (scaledW > sw) scaledW = sw;
    if (scaledH > sh) scaledH = sh;
  }
#endif
  int offX = (sw - scaledW) / 2;
  int offY = (sh - scaledH) / 2;

#if defined(__AVR__) || defined(ESP8266)
  const uint16_t* frameData =
      reinterpret_cast<const uint16_t*>(pgm_read_ptr(&anim.frames[frameIndex]));
#else
  const uint16_t* frameData = anim.frames[frameIndex];
#endif
  for (int y = 0; y < sh; y++) {
    int sy = y - offY;
    if (sy < 0 || sy >= scaledH) continue;
    int srcY = (sy * anim.height) / scaledH;
    if (srcY >= anim.height) srcY = anim.height - 1;
    for (int x = 0; x < sw; x++) {
      int sx = x - offX;
      if (sx < 0 || sx >= scaledW) continue;
      int srcX = (sx * anim.width) / scaledW;
      if (srcX >= anim.width) srcX = anim.width - 1;
      uint16_t color = pgm_read_word(&frameData[srcY * anim.width + srcX]);
      color = catAnimProcessColor(color);
#if CAT_ANIM_USE_PUSH_IMAGE
      if (usePush)
        s_push565Buf[(size_t)y * (size_t)sw + (size_t)x] = color;
      else
#endif
        sprite.drawPixel(x, y, color);
    }
  }
#if CAT_ANIM_USE_PUSH_IMAGE
  if (usePush) sprite.pushImage(0, 0, sw, sh, s_push565Buf);
#endif
}

inline void catAnimReset() {
  s_currentAnim = nullptr;
  s_currentFrame = 0;
  s_animHasBitmap = false;
}

inline bool catAnimBeginFS() {
  catAnimReset();
  catAnimScanAnimationsFolder();
  if (g_catCharacterIndex < 0 || g_catCharacterIndex >= kCatCharacterCount) {
    g_catCharacterIndex = CAT_DEFAULT_CHARACTER_INDEX;
  }
  catAnimEnsureDefaultPath();
  return true;
}

inline void catAnimOnPathChanged() { catAnimReset(); }

/** OBD(또는 시뮬 속도) km/h — 0이면 정지 계열 슬롯, 0 초과면 주행 계열 */
inline void catAnimSetObdKmh(int kmh) { g_catAnimObdKmh = kmh; }

inline void catAnimSetCharacterIndex(int idx) {
  if (idx < 0 || idx >= kCatCharacterCount) return;
  g_catCharacterIndex = idx;
  catAnimOnPathChanged();
}

inline void catAnimDraw(TFT_eSprite& sprite, uint16_t bgColor, bool advanceFrame) {
  const CatAnimAsset* anim = catAnimLookupCurrent();
  if (!anim) {
    sprite.fillSprite(bgColor);
    sprite.setTextColor(0x0000, bgColor);
    sprite.setTextSize(1);
    sprite.setCursor(4, (int)g_catSprH / 2 - 6);
    sprite.print(F("Animation not found"));
    return;
  }

  if (s_currentAnim != anim) {
    s_currentAnim = anim;
    s_currentFrame = 0;
    s_animHasBitmap = false;
  }

  if (!advanceFrame && s_animHasBitmap) return;

  catAnimDrawFrame(sprite, *anim, s_currentFrame, bgColor);
  s_animHasBitmap = true;

  if (advanceFrame && anim->frameCount > 0) {
    s_currentFrame = (s_currentFrame + 1) % anim->frameCount;
  }
}
