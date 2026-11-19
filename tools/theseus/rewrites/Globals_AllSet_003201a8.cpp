// True if the globals at $gp - 0x7618 (0x3F84D8), 0x3F92E8 and 0x3F92EC are all nonzero,
// tested in that order (4 call sites).
#include "theseus.h"

void Globals_AllSet_003201a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.gp() - 0x7618) && g.read32(0x3F92E8) && g.read32(0x3F92EC) ? 1 : 0);
}
