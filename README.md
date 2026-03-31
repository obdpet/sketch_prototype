# OBDPet 🐱

![Arduino](https://img.shields.io/badge/Arduino-IDE_2.3.8-00979D?style=flat&logo=arduino&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-WROOM--32-E7352C?style=flat&logo=espressif&logoColor=white)
![TFT_eSPI](https://img.shields.io/badge/TFT__eSPI-ST7789-blue?style=flat)
![License](https://img.shields.io/badge/License-MIT-green?style=flat)
![Status](https://img.shields.io/badge/Status-In_Development-yellow?style=flat)

---

## 한국어

### 프로젝트 개요

차량 OBD-II 포트에서 실시간 속도를 읽어 TFT 디스플레이 위의 고양이 캐릭터 애니메이션 속도에 매핑하는 IoT 디바이스.  
macOS의 **RunCat** (CPU 사용량 → 고양이 달리기 속도)에서 영감을 받아, 자동차 대시보드 버전으로 포팅한 프로젝트입니다.

### 하드웨어 (BOM)

| 부품 | 모델 |
|------|------|
| MCU | ELEGOO ESP32 개발보드 (WROOM-32, CP2102, USB-C) |
| OBD2 어댑터 | Veepeak Mini Bluetooth OBD2 (VP11) — BT Classic SPP, PIN: 1234 |
| 디스플레이 | Waveshare 2.0" LCD 240×320 ST7789 IPS |
| 기타 | 브레드보드 400핀, 점퍼선 M-M / M-F / F-F |

### 배선

**ESP32 ↔ Waveshare 2.0" ST7789**

| ESP32 GPIO | Waveshare 핀 |
|------------|-------------|
| GPIO 23 (MOSI) | SDA |
| GPIO 18 (SCK) | SCL |
| GPIO 2 | DC |
| GPIO 15 | CS |
| GPIO 4 | RST |
| 3.3V | VCC, BL |
| GND | GND |

### 소프트웨어 스택

- **Arduino IDE** 2.3.8
- **ESP32 보드 패키지**: esp32 by Espressif Systems 3.3.7
- **TFT_eSPI** (ST7789_DRIVER) — 디스플레이 드라이버
- **TFT_eSprite** — 더블버퍼링 (깜빡임 방지)
- **ELMduino** — OBD2 통신 (연동 예정)
- **BluetoothSerial** — BT Classic SPP (연동 예정)

### 펌웨어 기능 (`sketch_prototype.ino`)

- `TFT_eSprite` 더블버퍼링으로 깜빡임 없는 애니메이션
- 가로 모드 (`setRotation(1)`, 320×240)
- 고양이 캐릭터 픽셀아트 스타일 (TFT_eSPI 도형 함수 기반)
- **CatState 상태머신**: `SLEEP` / `WALK` / `FAST_WALK` / `RUN` / `TURBO` / `BRAKE`
- 속도 → 상태 매핑 및 프레임 딜레이 자동 계산
- Serial Monitor에서 `0~120` 숫자 입력으로 속도 실시간 시뮬레이션 가능

#### 속도 → 상태 매핑

| 속도 (km/h) | 상태 |
|-------------|------|
| 0 | SLEEP |
| 1 – 20 | WALK |
| 21 – 50 | FAST_WALK |
| 51 – 80 | RUN |
| 81 – 120 | TURBO |
| 감속 중 | BRAKE |

### 개발 환경 세팅 요약

1. Arduino IDE 2.3.8 설치
2. ESP32 보드 패키지 (Espressif 3.3.7) 설치
3. 라이브러리 설치: `TFT_eSPI`, `ELMduino`, `ESPAsyncWebServer`, `AsyncTCP`
4. `TFT_eSPI/User_Setup.h` 설정 (`ST7789_DRIVER`, 240×320, 핀 매핑)
5. ESP32 브레드보드 배선 완료 및 TFT 화면 출력 확인

### 다음 단계 (TODO)

- [ ] Veepeak OBD2 블루투스 페어링 및 실시간 속도 수신 연동
- [ ] 차량 실차 테스트
- [ ] 고양이 캐릭터 디자인 개선
- [ ] 커스텀 이미지 업로드 기능 (SPIFFS + WiFi AP)
- [ ] React Native 모바일 앱 개발

---

## English

### Project Overview

An IoT device that reads real-time vehicle speed from the OBD-II port and maps it to the animation speed of a cat character on a TFT display.  
Inspired by **RunCat** on macOS (CPU usage → cat running speed), ported to the car dashboard.

### Hardware (BOM)

| Component | Model |
|-----------|-------|
| MCU | ELEGOO ESP32 Dev Board (WROOM-32, CP2102, USB-C) |
| OBD2 Adapter | Veepeak Mini Bluetooth OBD2 (VP11) — BT Classic SPP, PIN: 1234 |
| Display | Waveshare 2.0" LCD 240×320 ST7789 IPS |
| Other | 400-pin breadboard, M-M / M-F / F-F jumper wires |

### Wiring

**ESP32 ↔ Waveshare 2.0" ST7789**

| ESP32 GPIO | Waveshare Pin |
|------------|---------------|
| GPIO 23 (MOSI) | SDA |
| GPIO 18 (SCK) | SCL |
| GPIO 2 | DC |
| GPIO 15 | CS |
| GPIO 4 | RST |
| 3.3V | VCC, BL |
| GND | GND |

### Software Stack

- **Arduino IDE** 2.3.8
- **ESP32 Board Package**: esp32 by Espressif Systems 3.3.7
- **TFT_eSPI** (ST7789_DRIVER) — Display driver
- **TFT_eSprite** — Double buffering (no flicker)
- **ELMduino** — OBD2 communication (pending)
- **BluetoothSerial** — BT Classic SPP (pending)

### Firmware Features (`sketch_prototype.ino`)

- Flicker-free animation via `TFT_eSprite` double buffering
- Landscape mode (`setRotation(1)`, 320×240)
- Pixel-art style cat character (drawn with TFT_eSPI shape functions)
- **CatState state machine**: `SLEEP` / `WALK` / `FAST_WALK` / `RUN` / `TURBO` / `BRAKE`
- Speed → state mapping with automatic frame delay calculation
- Real-time speed simulation via Serial Monitor (`0–120` input)

#### Speed → State Mapping

| Speed (km/h) | State |
|--------------|-------|
| 0 | SLEEP |
| 1 – 20 | WALK |
| 21 – 50 | FAST_WALK |
| 51 – 80 | RUN |
| 81 – 120 | TURBO |
| Decelerating | BRAKE |

### Environment Setup

1. Install Arduino IDE 2.3.8
2. Install ESP32 board package (Espressif 3.3.7)
3. Install libraries: `TFT_eSPI`, `ELMduino`, `ESPAsyncWebServer`, `AsyncTCP`
4. Configure `TFT_eSPI/User_Setup.h` (`ST7789_DRIVER`, 240×320, pin mapping)
5. Wire ESP32 on breadboard and verify TFT display output

### Roadmap (TODO)

- [ ] Pair Veepeak OBD2 via Bluetooth and receive live speed data
- [ ] Real vehicle testing
- [ ] Improve cat character design
- [ ] Custom image upload (SPIFFS + WiFi AP)
- [ ] React Native mobile app

---

## License

MIT License — see [LICENSE](LICENSE) for details.
