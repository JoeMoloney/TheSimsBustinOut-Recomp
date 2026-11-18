// Item i counted from the top of a {u32 count; u32 items[]} stack: items[count - 1 - i]
// (49 callers). Replaces FUN_00234c20 (stack = a0, i = a1). No bounds check, as in the original.
#include "theseus.h"

void Stack_PeekFromTop_00234c20(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t stack = g.arg(0);
    const uint32_t count = g.read32(stack);
    g.ret(g.read32(stack + 4 + (count - g.arg(1) - 1) * 4));
}
