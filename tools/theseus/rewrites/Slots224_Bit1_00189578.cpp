// Bit 1 of word +0x18 of the object in slot a1 of the pointer table at +0x224 of a0 (4 call sites).
#include "theseus.h"

void Slots224_Bit1_00189578(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.read32(g.arg(0) + (g.arg(1) << 2) + 0x224);
    const uint32_t flags = g.read32(obj + 0x18);  // gate: equivalent read16: only bit 1 is used
    g.ret((flags >> 1) & 1);
}
