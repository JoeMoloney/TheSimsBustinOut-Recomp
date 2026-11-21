// Reads a signed LEB128 value at a0 (7 bits per byte, bit 7 = more; shift counts wrap at 32 as `sllv` does; sign-extended from bit 6 of the last byte while the shift is below 32), stores it to *a1 and returns the address after it. 1 call site. Pairs with Varint7_Read_0022b880.
#include "theseus.h"

void Sleb128_Read_0022b8c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t p = g.arg(0), shift = 0, value = 0;
    uint8_t byte;
    do
    {
        byte = g.read8(p++);  // gate: equivalent read16: kept as a byte
        value |= static_cast<uint32_t>(byte & 0x7F) << (shift & 31);
        shift += 7;
    } while (byte & 0x80);
    if (shift < 32 && (byte & 0x40))
        value |= 0xFFFFFFFFu << shift;
    g.write32(g.arg(1), value);
    g.ret(p);
}
