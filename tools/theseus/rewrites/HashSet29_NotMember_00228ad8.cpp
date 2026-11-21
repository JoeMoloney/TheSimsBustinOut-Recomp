// 0 if a0 (all 64 bits) is one of 29 hash values, else 1. The original is the compiled switch:
// a binary search on signed 64-bit compares of a0 against the sign-extended constants (listed
// here in that order). 5 call sites (the callers pass the hash at +0x18 of an entry).
#include "theseus.h"

#include <algorithm>

void HashSet29_NotMember_00228ad8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    static constexpr uint32_t kHashes[] = {
        0x81E6BEF9, 0x865A6812, 0x86FDCC05, 0x8963D7C1, 0x950F8387, 0x950FAC20,
        0x950FD31A, 0xA4258067, 0xAED879C5, 0xBF1C5EFC, 0xDA2D322B, 0xE25246D2,
        0xEF121974, 0xF55676F7, 0x1CD89442, 0x31437EA7, 0x3EEC206C, 0x3FD182D0,
        0x4A0C562F, 0x4D122338, 0x4D1298FF, 0x4D129F56, 0x50907E06, 0x529D2B89,
        0x61B89983, 0x6660026F, 0x6ACAF6AD, 0x70F69082, 0x729C4842};
    const int64_t key = g.sarg64(0);
    const bool member = std::any_of(std::begin(kHashes), std::end(kHashes),
                                    [&](uint32_t h) { return static_cast<int32_t>(h) == key; });
    g.ret(member ? 0 : 1);
}
