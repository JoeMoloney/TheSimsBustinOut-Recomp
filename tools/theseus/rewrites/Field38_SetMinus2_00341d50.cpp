// Sets the word at a0+0x38 to -2 and returns -2. 1 call site. (The map joined another setter after the `jr $ra`.)
#include "theseus.h"

void Field38_SetMinus2_00341d50(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x38, 0xFFFFFFFEu);
    g.ret(0xFFFFFFFEu);
}
