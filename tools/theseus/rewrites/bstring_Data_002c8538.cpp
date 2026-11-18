// bstring: the first field of a string's representation (string->rep->field0).
// Replaces FUN_002c8538 (3 instructions; the function map lists 34 because unreachable padding
// follows it). rep +0 is probably the character buffer: the bstring.cpp constructor
// (FUN_002c85e0) stores the length at +4 and +8 and allocates length + 1 bytes.
#include "theseus.h"

void bstring_Data_002c8538(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t rep = g.read32(g.arg(0));
    g.ret(g.read32(rep));
}
