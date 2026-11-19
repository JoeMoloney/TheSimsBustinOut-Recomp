// True if the byte at a0 and the word at a1 share a set bit (only the byte's 8 bits can) (4 call sites).
#include "theseus.h"

void ByteMaskTest_00375a90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t mask = g.read32(g.arg(1));  // gate: equivalent read16: only the low byte meets the byte
    g.ret((g.read8(g.arg(0)) & mask) != 0 ? 1 : 0);
}
