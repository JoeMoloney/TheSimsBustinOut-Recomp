// Reads the next byte of a buffer (+0x20 data, +0x30 position) and advances the position.
// When the aligned flag (+0x38) is 0 the stream sits half a byte in: the byte is built from
// the low nibble of data[pos] and the high nibble of data[pos + 1] (read after the position
// is stored, as in the original). 4 call sites.
#include "theseus.h"

void Bitstream_ReadByte_0013a428(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t s = g.arg(0), pos = g.read32(s + 0x30);
    if (g.read32(s + 0x38) != 0)
    {
        const uint8_t b = g.read8(g.read32(s + 0x20) + pos);  // gate: equivalent read16: kept as a byte
        g.write32(s + 0x30, pos + 1);
        g.ret(b);
        return;
    }
    const uint32_t data = g.read32(s + 0x20);
    const uint8_t hi = g.read8(data + pos);  // gate: equivalent read16: kept as a byte
    g.write32(s + 0x30, pos + 1);
    const uint8_t lo = g.read8(data + pos + 1);  // gate: equivalent read16: kept as a byte
    g.ret(static_cast<uint32_t>((hi & 0xF) << 4) | (lo >> 4));
}
