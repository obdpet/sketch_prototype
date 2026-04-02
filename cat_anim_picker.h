#pragma once

/*
  빌드 시 포함된 애니메이션 목록을 다룹니다.
  시리얼: list | n번호 | n파일명.gif
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "cat_anim_profile.h"
#include "cat_anim_registry.h"

extern char g_animGifPath[80];

static bool leafEqualsIgnoreCase(const char* a, const char* b) {
  while (*a && *b) {
    if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return false;
    a++;
    b++;
  }
  return *a == *b;
}

inline int catAnimScanAnimationsFolder() {
  Serial.print(F("Built-in GIFs: "));
  Serial.println(kCatAnimationCount);
  return kCatAnimationCount;
}

inline void catAnimPrintAnimationList() {
  for (int i = 0; i < kCatAnimationCount; i++) {
    Serial.print(i);
    Serial.print(F(": "));
    Serial.println(kCatAnimations[i].name);
  }
  Serial.print(F("current: "));
  Serial.println(g_animGifPath);
}

inline bool catAnimSelectAnimationIndex(int idx) {
  if (idx < 0 || idx >= kCatAnimationCount) return false;
  strncpy(g_animGifPath, kCatAnimations[idx].name, sizeof(g_animGifPath) - 1);
  g_animGifPath[sizeof(g_animGifPath) - 1] = '\0';
  Serial.print(F("selected "));
  Serial.println(g_animGifPath);
  return true;
}

inline bool catAnimSelectAnimationByLeafName(const char* leaf) {
  if (!leaf || !leaf[0]) return false;
  for (int i = 0; i < kCatAnimationCount; i++) {
    if (leafEqualsIgnoreCase(kCatAnimations[i].name, leaf)) return catAnimSelectAnimationIndex(i);
  }
  Serial.print(F("not found: "));
  Serial.println(leaf);
  return false;
}

inline void catAnimEnsureDefaultPath() {
  if (g_animGifPath[0] != '\0') return;
  strncpy(g_animGifPath, CAT_DEFAULT_GIF_LEAF, sizeof(g_animGifPath) - 1);
  g_animGifPath[sizeof(g_animGifPath) - 1] = '\0';
}
