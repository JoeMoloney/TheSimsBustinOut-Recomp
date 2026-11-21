// Reads the next 4 bits of a buffer (+0x20 data, +0x30 byte position, +0x38 half flag): with the flag set, the high nibble of data[pos] (flag cleared); else the low nibble, the position advanced and the flag set. 3 call sites. Pairs with Bitstream_ReadByte_0013a428.
#include "theseus.h"

void Bitstream_ReadNibble_0013a3d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t s = g.arg(0), pos = g.read32(s + 0x30);
    if (g.read32(s + 0x38) != 0)
    {
        const uint8_t b = g.read8(g.read32(s + 0x20) + pos);  // gate: equivalent read16: kept as a byte
        g.write32(s + 0x38, 0);
        g.ret(b >> 4);
        return;
    }
    const uint8_t b = g.read8(g.read32(s + 0x20) + pos);  // gate: equivalent read16: kept as a byte
    g.write32(s + 0x30, pos + 1);
    g.write32(s + 0x38, 1);  // gate: equivalent write16: the flag word was 0 on this path, so its upper half stays 0
    g.ret(b & 0xF);
}
