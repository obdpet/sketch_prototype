#pragma once

/*
  플래시 절약: 펌웨어에 넣을 캐릭터 1종만 선택
  0 = mario + nyan + cjemoj 전부 포함(개발용)
  1 = mario만  2 = nyan만  3 = cjemoj만
  이 값에 맞춰 gif_to_rgb565.py 로 해당 폴더 GIF만 헤더 생성해도 되고,
  generated/<캐릭터>/*.h 가 있어도 컴파일러는 포함된 헤더만 링크합니다.
*/
#ifndef CAT_EMBED_PACK
#define CAT_EMBED_PACK 1
#endif

// 기본 애니메이션 파일명
#ifndef CAT_ANIMATIONS_DIR
#define CAT_ANIMATIONS_DIR "/animations"
#endif

// TFT_eSPI User_Setup의 TFT_RGB_ORDER와 중복되면 이중 스왑됨(모자 하늘색·바지 노랑 등). 먼저 0, 모자·바지만 여전히 바뀌면 1.
#ifndef CAT_ANIM_SWAP_RB
#define CAT_ANIM_SWAP_RB 0
#endif

// XOR 반전은 R/B 뒤집힘과 다름. 일반적으로 0.
#ifndef CAT_ANIM_INVERT_COLORS
#define CAT_ANIM_INVERT_COLORS 0
#endif

// 100=원본. 70~90이면 전체 톤을 어둡게(너무 밝은 패널·백라이트 대응).
#ifndef CAT_ANIM_DIM_PERCENT
#define CAT_ANIM_DIM_PERCENT 85
#endif

// 0: drawPixel (RAM 한 덩어리, 스프라이트만). 1: RAM에 w*h uint16_t 채운 뒤 sprite.pushImage 한 번(스프라이트 2배 RAM).
// 맞춤: buf는 행 우선 RGB565, 픽셀당 uint16_t. tft/sprite의 setSwapBytes는 drawPixel과 동일하게 유지.
// 색 깨지면 User_Setup TFT_RGB_ORDER·CAT_ANIM_SWAP_RB·swapBytes 조합을 drawPixel 모드에서 맞춘 뒤 동일하게 적용.
#ifndef CAT_ANIM_USE_PUSH_IMAGE
#define CAT_ANIM_USE_PUSH_IMAGE 0
#endif

#ifndef CAT_DEFAULT_GIF_LEAF
#define CAT_DEFAULT_GIF_LEAF "nyan-cat-rainbow.gif"
#endif

#ifndef CAT_DEFAULT_CHARACTER_INDEX
#define CAT_DEFAULT_CHARACTER_INDEX 1
#endif

#if CAT_EMBED_PACK >= 1 && CAT_EMBED_PACK <= 3
#undef CAT_DEFAULT_CHARACTER_INDEX
#define CAT_DEFAULT_CHARACTER_INDEX 0
#endif

// 0: cover — 스프라이트를 꽉 채우고 넘치는 부분 잘림(비율 유지·중앙 크롭).
// 1: contain — 캐릭터 전체가 보이도록 축소 후 화면 정중앙(레터박스).
#ifndef CAT_ANIM_FIT_CONTAIN
#define CAT_ANIM_FIT_CONTAIN 1
#endif

// contain 여백을 BG_COLOR 대신 애니메이션 0번 프레임 모서리 평균색으로 채움(하늘색 등에 가깝게).
#ifndef CAT_ANIM_LETTERBOX_FROM_ANIM_BORDER
#define CAT_ANIM_LETTERBOX_FROM_ANIM_BORDER 1
#endif
