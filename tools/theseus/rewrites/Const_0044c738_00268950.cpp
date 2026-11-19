// Returns the constant address 0x44C738 (a global object; 51 call sites over 3 identical copies).
// theseus: copies 0x2833e8, 0x2856f8
#include "theseus.h"

void Const_0044c738_00268950(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(0x0044C738);
}
