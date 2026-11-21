// Returns entry a0 - 1 of the 16-word table at 0x3B3A88, the index clamped to 0..15. (The
// original copies the table to its stack frame first and reads the copy.) 7 call sites.
#include "theseus.h"

void Table3B3A88_ClampedAt_00145910(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    int32_t index = static_cast<int32_t>(g.arg(0) - 1);
    if (index < 0)  // gate: equivalent <=,1: index 0 becomes 0 either way
        index = 0;
    if (index > 15)  // gate: equivalent >=: index 15 stays 15 either way
        index = 15;
    g.ret(g.read32(0x3B3A88 + (static_cast<uint32_t>(index) << 2)));
}
