// Initialises the tree header at a0 (see Tree18_FindOrParent_002b4b80): +4 = 0, root +8 = the
// shared nil node 0x3A8AA8, +0 = 0; returns a0 (all 64 bits). 15 call sites.
#include "theseus.h"

void Tree18_InitEmpty_002b4900(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, 0);
    g.write32(g.arg(0) + 8, 0x003A8AA8);
    g.write32(g.arg(0), 0);
    g.ret_copy_of_arg(0);
}
