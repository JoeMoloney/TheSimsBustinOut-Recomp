// std::find over 16-bit elements: the first element in [a0, a1) equal to *a2, else a1 (unrolled by
// four, then the 0..3 left over). Returns the pointer as the original's `daddu v0, a0/a1` leaves
// it: a0 or a1 unchanged (low 64 bits) when it never moved, else sign-extended. 1 call site.
#include "theseus.h"

void S16_Find_002c8248(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t value = g.sread16(g.arg(2));
    uint32_t p = g.arg(0);
    const auto found = [&]() {
        g.ret64(p == g.arg(0) ? g.arg64(0) : static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(p))));  // gate: equivalent 1: arg(1) only changes bits 32..63 of v0, not part of the ABI compare
    };
    for (int32_t groups = static_cast<int32_t>(g.arg(1) - p) >> 3; groups > 0; --groups)
        for (int k = 0; k < 4; ++k, p += 2)
            if (g.sread16(p) == value)
            {
                found();
                return;
            }
    switch (static_cast<int32_t>(g.arg(1) - p) >> 1)
    {
    case 3:
        if (g.sread16(p) == value)
        {
            found();
            return;
        }
        p += 2;
        [[fallthrough]];
    case 2:
        if (g.sread16(p) == value)
        {
            found();
            return;
        }
        p += 2;
        [[fallthrough]];
    case 1:
        if (g.sread16(p) == value)
        {
            found();
            return;
        }
        break;
    default:
        break;
    }
    g.ret64(g.arg64(1));
}
