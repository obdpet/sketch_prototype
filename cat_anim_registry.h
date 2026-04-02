#pragma once

#include "cat_anim_types.h"
#include "generated/anim_cjemoj.h"
#include "generated/anim_mario_16_bit.h"
#include "generated/anim_nyan_cat_rainbow.h"

static const CatAnimAsset kCatAnimations[] = {
    {mario_16_bit_name, mario_16_bit_width, mario_16_bit_height, mario_16_bit_frame_count, mario_16_bit_frames},
    {nyan_cat_rainbow_name, nyan_cat_rainbow_width, nyan_cat_rainbow_height, nyan_cat_rainbow_frame_count,
     nyan_cat_rainbow_frames},
    {cjemoj_name, cjemoj_width, cjemoj_height, cjemoj_frame_count, cjemoj_frames},
};

static constexpr int kCatAnimationCount = sizeof(kCatAnimations) / sizeof(kCatAnimations[0]);
