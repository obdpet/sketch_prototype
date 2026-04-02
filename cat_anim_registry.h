#pragma once

#include "cat_anim_types.h"

#ifndef CAT_EMBED_PACK
#define CAT_EMBED_PACK 0
#endif

#if CAT_EMBED_PACK == 0

#include "generated/cjemoj/anim_cjemoj.h"
#include "generated/cjemoj/anim_cjemoj_sleep.h"
#include "generated/mario/anim_mario_16_bit.h"
#include "generated/mario/anim_mario_sleep.h"
#include "generated/nyan/anim_nyan_cat_rainbow.h"

static const CatAnimAsset kAssetMarioSleep = {mario_sleep_name, mario_sleep_width, mario_sleep_height,
                                              mario_sleep_frame_count, mario_sleep_frames};

static const CatAnimAsset kAssetMarioRun = {mario_16_bit_name, mario_16_bit_width, mario_16_bit_height,
                                            mario_16_bit_frame_count, mario_16_bit_frames};

static const CatAnimAsset kAssetNyanRun = {nyan_cat_rainbow_name, nyan_cat_rainbow_width, nyan_cat_rainbow_height,
                                           nyan_cat_rainbow_frame_count, nyan_cat_rainbow_frames};

static const CatAnimAsset kAssetCjemojSleep = {cjemoj_sleep_name, cjemoj_sleep_width, cjemoj_sleep_height,
                                               cjemoj_sleep_frame_count, cjemoj_sleep_frames};

static const CatAnimAsset kAssetCjemojRun = {cjemoj_name, cjemoj_width, cjemoj_height, cjemoj_frame_count,
                                             cjemoj_frames};

static const CatCharacterPack kCatCharacters[] = {
    {"mario", {&kAssetMarioSleep, &kAssetMarioRun, nullptr, nullptr}},
    {"nyan", {nullptr, &kAssetNyanRun, nullptr, nullptr}},
    {"cjemoj", {&kAssetCjemojSleep, &kAssetCjemojRun, nullptr, nullptr}},
};

#elif CAT_EMBED_PACK == 1

#include "generated/mario/anim_mario_16_bit.h"
#include "generated/mario/anim_mario_sleep.h"

static const CatAnimAsset kAssetMarioSleep = {mario_sleep_name, mario_sleep_width, mario_sleep_height,
                                              mario_sleep_frame_count, mario_sleep_frames};

static const CatAnimAsset kAssetMarioRun = {mario_16_bit_name, mario_16_bit_width, mario_16_bit_height,
                                            mario_16_bit_frame_count, mario_16_bit_frames};

static const CatCharacterPack kCatCharacters[] = {
    {"mario", {&kAssetMarioSleep, &kAssetMarioRun, nullptr, nullptr}},
};

#elif CAT_EMBED_PACK == 2

#include "generated/nyan/anim_nyan_cat_rainbow.h"

static const CatAnimAsset kAssetNyanRun = {nyan_cat_rainbow_name, nyan_cat_rainbow_width, nyan_cat_rainbow_height,
                                           nyan_cat_rainbow_frame_count, nyan_cat_rainbow_frames};

static const CatCharacterPack kCatCharacters[] = {
    {"nyan", {nullptr, &kAssetNyanRun, nullptr, nullptr}},
};

#elif CAT_EMBED_PACK == 3

#include "generated/cjemoj/anim_cjemoj.h"
#include "generated/cjemoj/anim_cjemoj_sleep.h"

static const CatAnimAsset kAssetCjemojSleep = {cjemoj_sleep_name, cjemoj_sleep_width, cjemoj_sleep_height,
                                               cjemoj_sleep_frame_count, cjemoj_sleep_frames};

static const CatAnimAsset kAssetCjemojRun = {cjemoj_name, cjemoj_width, cjemoj_height, cjemoj_frame_count,
                                             cjemoj_frames};

static const CatCharacterPack kCatCharacters[] = {
    {"cjemoj", {&kAssetCjemojSleep, &kAssetCjemojRun, nullptr, nullptr}},
};

#else
#error "CAT_EMBED_PACK must be 0 (all), 1 (mario), 2 (nyan), or 3 (cjemoj). Add a branch when adding characters."
#endif

static constexpr int kCatCharacterCount = sizeof(kCatCharacters) / sizeof(kCatCharacters[0]);
