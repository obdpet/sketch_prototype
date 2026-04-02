// RGB565 애니메이션. 시리얼: 속도(km/h)→OBD 흉내(catAnimSetObdKmh), list/clist, c<캐릭터>
//
// [240×320 패널 + 오른쪽 노이즈]
// TFT_eSPI User_Setup.h: ST7789 기준 물리 픽셀은 보통
//   #define TFT_WIDTH  240
//   #define TFT_HEIGHT 320
// (세로가 긴 본래 방향). 가로·세로를 바꿔 쓰거나 다른 해상도(예: 320×240 칩용 설정)를 쓰면
// 주소 창이 어긋나 한쪽만 잡음·깨짐이 납니다. setRotation 후 시리얼의 TFT: WxH가
// 그때의 논리 크기(landscape면 320×240 등)와 스프라이트가 일치하는지 확인하세요.
//
#include <cctype>
#include <stdio.h>
#include <string.h>
#include <TFT_eSPI.h>
#include "cat_anim_profile.h"
#include "cat_animation.h"

char g_animGifPath[80] = "";
int g_catCharacterIndex = CAT_DEFAULT_CHARACTER_INDEX;
int g_catAnimObdKmh = 0;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

// 순백(0xFFFF)보다 살짝 어두운 배경 — 백라이트가 강한 모듈에서 덜 눈부심
#define BG_COLOR 0xFFFF

#ifndef TFT_ROTATION
// 0=세로, 1=가로(시계 90°), 2=세로 뒤집음, 3=가로 반대(반시계 90°)
#define TFT_ROTATION 1
#endif

uint16_t g_screenW = 240;
uint16_t g_screenH = 320;
uint16_t g_catSprW = 240;
uint16_t g_catSprH = 320;
int g_sprX = 0;
int g_sprY = 0;

static void setupDisplay() {
  tft.init();
  tft.setRotation(TFT_ROTATION);
}

static void layoutScreenAndSpriteArea() {
  g_screenW = (uint16_t)tft.width();
  g_screenH = (uint16_t)tft.height();

  g_catSprW = g_screenW;
  g_catSprH = g_screenH ? g_screenH : 1u;

  g_sprX = 0;
  g_sprY = 0;
}

// RAM 부족 대응
static bool createAnimSpriteWithFallback() {
  layoutScreenAndSpriteArea();
  sprite.setColorDepth(16);

  if (sprite.createSprite(g_catSprW, g_catSprH)) return true;

  while (g_catSprW >= 80 && g_catSprH >= 60) {
    g_catSprW = g_catSprW * 4 / 5;
    g_catSprH = g_catSprH * 4 / 5;

    g_sprX = (int)(g_screenW - g_catSprW) / 2;
    g_sprY = (int)(g_screenH - g_catSprH) / 2;

    if (sprite.createSprite(g_catSprW, g_catSprH)) return true;
  }

  return false;
}

static constexpr int kSpeedMax = 120;

int currentSpeed = 30;
unsigned long lastFrameTime = 0;

static bool isSpriteFullBleed() {
  return (g_sprX == 0 && g_sprY == 0 && g_catSprW >= g_screenW && g_catSprH >= g_screenH);
}

/** 스프라이트가 화면보다 작을 때만, 시작 시 한 번 TFT 여백 채움(매 프레임 fillScreen 금지). */
static void paintTftGuttersOnce() {
  if (!isSpriteFullBleed()) tft.fillScreen(catAnimGetBorderColorStable(BG_COLOR));
}

// 폰트4 기준 "Speed:200" 넉넉히 덮음 — 이전 자릿수 잔상 방지
static constexpr int kHudX = 4;
static constexpr int kHudY = 4;
static constexpr int kHudW = 132;
static constexpr int kHudH = 34;

/** 왼쪽 상단 현재 속도(기본 폰트는 ASCII만 안정적이라 라벨은 영문 축약). */
static void drawSpeedHudOnSprite() {
  sprite.fillRect(kHudX, kHudY, kHudW, kHudH, BG_COLOR);
  char buf[20];
  snprintf(buf, sizeof(buf), "Speed:%3d", currentSpeed);
  sprite.setTextFont(4);
  sprite.setTextDatum(TL_DATUM);
  sprite.setTextColor(TFT_WHITE, BG_COLOR);
  sprite.drawString(buf, kHudX, kHudY);
}

static void drawSpeedHudOnTft() {
  uint16_t bg = catAnimGetBorderColorStable(BG_COLOR);
  tft.fillRect(kHudX, kHudY, kHudW, kHudH, bg);
  char buf[20];
  snprintf(buf, sizeof(buf), "Speed:%3d", currentSpeed);
  tft.setTextFont(4);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, bg);
  tft.drawString(buf, kHudX, kHudY);
}

static void presentAnimFrame() {
  catAnimDraw(sprite, BG_COLOR, true);
  if (isSpriteFullBleed()) drawSpeedHudOnSprite();
  sprite.pushSprite(g_sprX, g_sprY);
  if (!isSpriteFullBleed()) drawSpeedHudOnTft();
}

void setup() {
  Serial.begin(115200);

  setupDisplay();

  tft.setSwapBytes(true);
  sprite.setSwapBytes(true);

  tft.fillScreen(BG_COLOR); 


  if (!createAnimSpriteWithFallback()) {
    Serial.println("Sprite fail (RAM 부족)");
  }

  Serial.printf("TFT: %dx%d (오른쪽 노이즈면 User_Setup TFT_WIDTH/HEIGHT)\n", tft.width(),
                tft.height());

  catAnimBeginFS();
  catAnimReset();

  paintTftGuttersOnce();
  presentAnimFrame();

  lastFrameTime = millis();
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      if (line.equalsIgnoreCase("list") || line.equalsIgnoreCase("clist")) {
        catAnimPrintAnimationList();
      } else if (line.length() > 1 && (line[0] == 'c' || line[0] == 'C')) {
        String rest = line.substring(1);
        rest.trim();
        if (rest.length() > 0 && isdigit((unsigned char)rest[0])) {
          if (catAnimSelectCharacterIndex(rest.toInt())) {
            catAnimOnPathChanged();
            paintTftGuttersOnce();
            presentAnimFrame();
          }
        }
      } else if (line.length() > 1 && (line[0] == 'n' || line[0] == 'N')) {
        String rest = line.substring(1);
        rest.trim();
        if (rest.length() > 0) {
          if (rest.indexOf('.') >= 0) {
            if (catAnimSelectAnimationByLeafName(rest.c_str())) {
              catAnimOnPathChanged();
              paintTftGuttersOnce();
              presentAnimFrame();
            }
          } else {
            if (catAnimSelectAnimationIndex(rest.toInt())) {
              catAnimOnPathChanged();
              paintTftGuttersOnce();
              presentAnimFrame();
            }
          }
        }
      } else if (line[0] == '-' || isdigit((unsigned char)line[0])) {
        currentSpeed = constrain(line.toInt(), 0, kSpeedMax);
      }
    }
  }

  catAnimSetObdKmh(currentSpeed);

  unsigned long now = millis();
  int delay_ms = map(currentSpeed, 1, kSpeedMax, 400, 40);

  if (now - lastFrameTime >= delay_ms) {
    lastFrameTime = now;

    presentAnimFrame();
  }
}