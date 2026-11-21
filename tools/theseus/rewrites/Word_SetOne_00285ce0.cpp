// Sets the word at a0 to 1 and returns 1. 1 call site. (The map joined other functions' epilogues after the `jr $ra`.)
#include "theseus.h"

void Word_SetOne_00285ce0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 1);
    g.ret(1);
}
