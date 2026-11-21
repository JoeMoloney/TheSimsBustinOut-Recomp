// Reads a little-endian base-128 varint at a0 (7 bits per byte, bit 7 = more bytes follow;
// shift counts wrap at 32 as `sllv` does), stores it to *a1 and returns the address after it.
// 7 call sites.
#include "theseus.h"

void Varint7_Read_0022b880(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t p = g.arg(0), shift = 0;
    uint8_t byte = g.read8(p++);  // gate: equivalent read16: kept as a byte
    uint32_t value = byte & 0x7F;
    while (byte & 0x80)
    {
        byte = g.read8(p++);  // gate: equivalent read16: kept as a byte
        shift += 7;
        value |= static_cast<uint32_t>(byte & 0x7F) << (shift & 31);
    }
    g.write32(g.arg(1), value);
    g.ret(p);
}
