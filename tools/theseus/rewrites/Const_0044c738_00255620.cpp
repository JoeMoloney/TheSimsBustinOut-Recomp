// Returns the constant address 0x44C738 (same as Const_0044c738_00268950; the function map
// joins the next function, `return 1` at 0x255630, so the bytes don't match). 17 call sites.
#include "theseus.h"

void Const_0044c738_00255620(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(0x0044C738);
}
