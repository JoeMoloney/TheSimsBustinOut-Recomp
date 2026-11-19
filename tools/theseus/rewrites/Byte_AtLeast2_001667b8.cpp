// True if the low byte of a1 is 2 or more (7 call sites).
#include "theseus.h"

void Byte_AtLeast2_001667b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret((g.arg(1) & 0xFF) >= 2 ? 1 : 0);
}
