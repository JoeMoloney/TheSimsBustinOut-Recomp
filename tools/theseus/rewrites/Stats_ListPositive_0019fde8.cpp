// Builds a list of up to 4 positive stats for the current slot of a0: slot = signed byte +0x18D6 (-1: none), entry = [a0 + 0xA0 + slot * 4] (0: none), stats = [entry + 0x18]. Of the seven 16-bit stats at stats+0xA4..+0xB0 (codes 1, 6, 2, 5, 3, 7, 8 in that order), each one > 0 is appended while fewer than 4 are listed: code at a0+0x1965 + n, low byte of the value at a0+0x1969 + n, count at a0+0x1964. Returns a0 + 0x1964, or 0 when there is no slot/entry. 3 call sites.
#include "theseus.h"

void Stats_ListPositive_0019fde8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.arg(0);
    SET_GPR_U64(ctx, 7, g.arg64(0));  // a3 = a0 (daddu), which some callers' next callee reads; gate: equivalent
    const int8_t slot = g.sread8(obj + 0x18D6);  // gate: equivalent read8: stored in an int8_t either way
    if (slot == -1)
    {
        g.ret(0);
        return;
    }
    const uint32_t entry = g.read32(obj + (static_cast<uint32_t>(static_cast<int32_t>(slot)) << 2) + 0xA0);
    if (entry == 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t stats = g.read32(entry + 0x18);
    SET_GPR_S32(ctx, 8, static_cast<int32_t>(stats));  // t0, likewise; gate: equivalent
    g.write8(obj + 0x1964, 0);
    static const uint8_t kCode[7] = {1, 6, 2, 5, 3, 7, 8};  // gate: equivalent 8: the array size
    uint32_t n = 0;
    for (uint32_t i = 0; i < 7; ++i)
    {
        const uint32_t field = stats + 0xA4 + 2 * i;
        if (static_cast<int16_t>(g.read16(field)) > 0 && n < 4)  // gate: equivalent read32: cast to int16_t
        {
            g.write8(obj + 0x1969 + n, g.read8(field));  // gate: equivalent read16: write8 keeps only the low byte
            g.write8(obj + 0x1965 + n, kCode[i]);
            ++n;
            g.write8(obj + 0x1964, static_cast<uint8_t>(n));
        }
    }
    g.ret(obj + 0x1964);
}
