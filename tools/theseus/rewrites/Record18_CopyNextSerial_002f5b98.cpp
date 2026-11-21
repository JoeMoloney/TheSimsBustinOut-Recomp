// Copies the record at a1 to a0 (+0, +0x10, 8 bytes at +4, +0x14, +0xC, in the original'\''s order), with +0x10 incremented unless it is -1. Returns a0 (v1 = the +0x14 word, a0 = the +0xC word). 1 call site.
#include "theseus.h"

void Record18_CopyNextSerial_002f5b98(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t dst = g.arg(0), src = g.arg(1);
    const uint64_t dst64 = g.arg64(0);
    g.write32(dst, g.read32(src));
    const uint32_t serial = g.read32(src + 0x10);
    g.write32(dst + 0x10, serial);
    if (serial != 0xFFFFFFFFu)
        g.write32(dst + 0x10, serial + 1);
    g.write64(dst + 4, g.read64(src + 4));
    const uint32_t w14 = g.read32(src + 0x14);
    g.write32(dst + 0x14, w14);
    const uint32_t wC = g.read32(src + 0xC);
    g.write32(dst + 0xC, wC);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(w14));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 4, static_cast<int32_t>(wC));  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 2, dst64);
    g.ret_void();
}
