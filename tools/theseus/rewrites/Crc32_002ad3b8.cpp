// CRC-32 of a1 bytes at a0 (reflected, initial value and final xor 0xFFFFFFFF), byte-wise with
// the 256-entry table at 0x3A8650. 8 call sites.
#include "theseus.h"

void Crc32_002ad3b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t p = g.arg(0);
    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t n = g.arg(1); n != 0; --n)
        crc = (crc >> 8) ^ g.read32(0x3A8650 + 4 * ((crc ^ g.read8(p++)) & 0xFF));  // gate: equivalent read16: only the low byte survives & 0xFF
    g.ret(~crc);
}
