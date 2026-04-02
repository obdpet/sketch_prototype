#pragma once

/*
  빌드에 포함된 캐릭터 팩 / 슬롯 목록.
  시리얼: list | clist | c번호 | n번호(캐릭터) | n파일명.gif
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "cat_anim_driver.h"
#include "cat_anim_profile.h"

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
  Serial.print(F("Characters: "));
  Serial.println(kCatCharacterCount);
  return kCatCharacterCount;
}

inline void catAnimPrintCharacterList() {
  Serial.println(F("characters (c<idx>):"));
  for (int i = 0; i < kCatCharacterCount; i++) {
    Serial.print(i);
    Serial.print(F(": "));
    Serial.print(kCatCharacters[i].id);
    Serial.print(F(" ["));
    for (int s = 0; s < CAT_ANIM_SLOT_COUNT; s++) {
      if (s) Serial.print(F(","));
      if (kCatCharacters[i].slot[s]) {
        Serial.print(kCatCharacters[i].slot[s]->name);
      } else {
        Serial.print(F("-"));
      }
    }
    Serial.println(F("]"));
  }
}

inline void catAnimPrintAnimationList() {
  catAnimPrintCharacterList();
  Serial.print(F("OBD km/h (playback): "));
  Serial.println(g_catAnimObdKmh);
  Serial.print(F("current clip: "));
  Serial.println(g_animGifPath);
}

inline bool catAnimSelectCharacterIndex(int idx) {
  if (idx < 0 || idx >= kCatCharacterCount) return false;
  g_catCharacterIndex = idx;
  Serial.print(F("character "));
  Serial.println(kCatCharacters[idx].id);
  return true;
}

/** 이전 API 호환: 인덱스는 캐릭터 팩 인덱스 */
inline bool catAnimSelectAnimationIndex(int idx) { return catAnimSelectCharacterIndex(idx); }

inline bool catAnimSelectAnimationByLeafName(const char* leaf) {
  if (!leaf || !leaf[0]) return false;
  for (int c = 0; c < kCatCharacterCount; c++) {
    for (int s = 0; s < CAT_ANIM_SLOT_COUNT; s++) {
      const CatAnimAsset* a = kCatCharacters[c].slot[s];
      if (!a) continue;
      if (!leafEqualsIgnoreCase(a->name, leaf)) continue;
      g_catCharacterIndex = c;
      if (s == CAT_ANIM_SLEEP || s == CAT_ANIM_IDLE)
        g_catAnimObdKmh = 0;
      else
        g_catAnimObdKmh = 1;
      Serial.print(F("character "));
      Serial.print(kCatCharacters[c].id);
      Serial.print(F(" clip "));
      Serial.println(a->name);
      return true;
    }
  }
  Serial.print(F("not found: "));
  Serial.println(leaf);
  return false;
}

inline void catAnimEnsureDefaultPath() {
  if (g_catCharacterIndex >= 0 && g_catCharacterIndex < kCatCharacterCount) return;
  g_catCharacterIndex = CAT_DEFAULT_CHARACTER_INDEX;
}
