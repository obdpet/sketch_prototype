#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

#define CAT_BODY   0xEC27
#define CAT_EAR    0xC326
#define CAT_EYE    0x0000
#define CAT_TONGUE 0xF800
#define SPEED_LINE 0xFE43
#define ZZZ_COLOR  0x8DDF
#define BG_COLOR   0xFFFF

#define SCREEN_W 320
#define SCREEN_H 240
#define SPR_W    160
#define SPR_H    140
#define SPR_X    ((SCREEN_W - SPR_W) / 2)
#define SPR_Y    ((SCREEN_H - SPR_H) / 2)
#define CAT_CX   (SPR_W / 2)
#define CAT_CY   (SPR_H / 2)

enum CatState { SLEEP, WALK, FAST_WALK, RUN, TURBO, BRAKE };

int currentSpeed = 0;
int prevSpeed = 0;
int frameIndex = 0;
CatState currentState = SLEEP;
unsigned long lastFrameTime = 0;
int lastDisplayedSpeed = -1;
CatState lastDisplayedState = SLEEP;

CatState mapSpeedToState(int speed, int prev) {
  if (prev - speed > 20) return BRAKE;
  if (speed == 0) return SLEEP;
  if (speed < 30) return WALK;
  if (speed < 60) return FAST_WALK;
  if (speed < 120) return RUN;
  return TURBO;
}

int calcFrameDelay(int speed) {
  if (speed == 0) return 500;
  return map(speed, 1, 120, 400, 40);
}

void drawUI() {
  if (currentSpeed != lastDisplayedSpeed || currentState != lastDisplayedState) {
    tft.fillRect(0, 0, SCREEN_W, 40, BG_COLOR);
    tft.setTextColor(0x0000);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("Speed: ");
    tft.print(currentSpeed);
    tft.print(" km/h");

    tft.setTextSize(1);
    tft.setCursor(220, 10);
    switch(currentState) {
      case SLEEP:     tft.print("SLEEP");     break;
      case WALK:      tft.print("WALK");      break;
      case FAST_WALK: tft.print("FAST WALK"); break;
      case RUN:       tft.print("RUN");       break;
      case TURBO:     tft.print("TURBO");     break;
      case BRAKE:     tft.print("BRAKE");     break;
    }

    tft.setTextSize(1);
    tft.setCursor(10, 30);
    tft.setTextColor(0x8410);
    tft.print("Serial Monitor: 0-120 입력후 Enter");

    lastDisplayedSpeed = currentSpeed;
    lastDisplayedState = currentState;
  }
}

void drawCatToSprite(CatState state, int frame) {
  sprite.fillSprite(BG_COLOR);

  int cx = CAT_CX;
  int cy = CAT_CY;

  if (state == BRAKE) {
    sprite.fillSprite(0xF800);
    sprite.setTextColor(0xFFFF);
    sprite.setTextSize(2);
    sprite.setCursor(20, cy-8);
    sprite.print("BRAKE!");
    return;
  }

  if (state == SLEEP) {
    sprite.fillEllipse(cx, cy+10, 32, 18, CAT_BODY);
    sprite.fillTriangle(cx-14, cy-8,  cx-24, cy-24, cx-2,  cy-8,  CAT_EAR);
    sprite.fillTriangle(cx+4,  cy-8,  cx+16, cy-24, cx+18, cy-6,  CAT_EAR);
    sprite.drawLine(cx-10, cy+2, cx-5,  cy+5, CAT_EYE);
    sprite.drawLine(cx+5,  cy+2, cx+10, cy+5, CAT_EYE);
    sprite.drawLine(cx-32, cy+10, cx-48, cy,    CAT_BODY);
    sprite.drawLine(cx-48, cy,    cx-50, cy-10, CAT_BODY);
    sprite.setTextColor(ZZZ_COLOR);
    sprite.setTextSize(2);
    if (frame % 2 == 0) {
      sprite.setCursor(cx+28, cy-25);
      sprite.print("z");
    } else {
      sprite.setCursor(cx+24, cy-35);
      sprite.print("zZ");
    }
    return;
  }

  int f = frame % 2;
  int bodyBob = (f == 0) ? 0 : -4;
  int legSpread = 16;
  if (state == FAST_WALK) legSpread = 20;
  if (state == RUN)       legSpread = 25;
  if (state == TURBO)     legSpread = 28;

  if (state == RUN || state == TURBO) {
    int lineCount = (state == TURBO) ? 3 : 2;
    for (int i = 0; i < lineCount; i++) {
      int ly = cy + 5 + bodyBob + i * 10;
      int lineLen = (state == TURBO) ? 45 : 28;
      sprite.drawLine(cx-40, ly, cx-40-lineLen, ly, SPEED_LINE);
    }
  }

  int bodyW = 30, bodyH = 15;
  if (state == RUN || state == TURBO) { bodyW = 34; bodyH = 12; }
  sprite.fillEllipse(cx, cy+bodyBob, bodyW, bodyH, CAT_BODY);
  sprite.fillCircle(cx+34, cy-8+bodyBob, 16, CAT_BODY);

  sprite.fillTriangle(cx+22, cy-22+bodyBob, cx+20, cy-38+bodyBob, cx+34, cy-22+bodyBob, CAT_EAR);
  sprite.fillTriangle(cx+34, cy-22+bodyBob, cx+44, cy-36+bodyBob, cx+48, cy-16+bodyBob, CAT_EAR);

  sprite.fillCircle(cx+40, cy-10+bodyBob, 3, CAT_EYE);

  if (state == TURBO) {
    sprite.fillEllipse(cx+50, cy-2+bodyBob, 5, 4, CAT_TONGUE);
  }

  int tailTipY = (state == RUN || state == TURBO) ? cy-35+bodyBob : cy-18+bodyBob;
  sprite.drawLine(cx-30, cy+bodyBob,   cx-48, tailTipY,   CAT_BODY);
  sprite.drawLine(cx-30, cy+1+bodyBob, cx-48, tailTipY+1, CAT_BODY);

  if (f == 0) {
    sprite.drawLine(cx+16, cy+12+bodyBob, cx+22, cy+12+legSpread+bodyBob, CAT_BODY);
    sprite.drawLine(cx+8,  cy+14+bodyBob, cx+2,  cy+14+legSpread+bodyBob, CAT_BODY);
  } else {
    sprite.drawLine(cx+16, cy+12+bodyBob, cx+10, cy+12+legSpread+bodyBob, CAT_BODY);
    sprite.drawLine(cx+8,  cy+14+bodyBob, cx+14, cy+14+legSpread+bodyBob, CAT_BODY);
  }

  if (f == 0) {
    sprite.drawLine(cx-14, cy+12+bodyBob, cx-8,  cy+12+legSpread+bodyBob, CAT_BODY);
    sprite.drawLine(cx-26, cy+10+bodyBob, cx-32, cy+10+legSpread+bodyBob, CAT_BODY);
  } else {
    sprite.drawLine(cx-14, cy+12+bodyBob, cx-20, cy+12+legSpread+bodyBob, CAT_BODY);
    sprite.drawLine(cx-26, cy+10+bodyBob, cx-20, cy+10+legSpread+bodyBob, CAT_BODY);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("OBDPet Ready!");
  Serial.println("속도 입력 (0-120) 후 Enter:");

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BG_COLOR);
  sprite.createSprite(SPR_W, SPR_H);
}

void loop() {
  // 시리얼 입력 처리
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    int newSpeed = input.toInt();
    newSpeed = constrain(newSpeed, 0, 120);
    prevSpeed = currentSpeed;
    currentSpeed = newSpeed;
    currentState = mapSpeedToState(currentSpeed, prevSpeed);
    Serial.print("Speed set: ");
    Serial.print(currentSpeed);
    Serial.println(" km/h");
  }

  // 프레임 업데이트
  unsigned long now = millis();
  int delay_ms = calcFrameDelay(currentSpeed);
  if (now - lastFrameTime > (unsigned long)delay_ms) {
    frameIndex = (frameIndex + 1) % 4;
    drawUI();
    drawCatToSprite(currentState, frameIndex);
    sprite.pushSprite(SPR_X, SPR_Y);
    lastFrameTime = now;
  }
}