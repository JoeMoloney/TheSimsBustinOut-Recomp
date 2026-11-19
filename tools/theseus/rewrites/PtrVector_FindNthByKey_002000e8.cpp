// The a2-th (from 0) object in the vector of object pointers from +8 (begin) to +0xC (end) of
// a0 whose first word equals a1 (64-bit compares, as `bne`); 0 if there are fewer. 19 call sites.
#include "theseus.h"

void PtrVector_FindNthByKey_002000e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    // a3 (matches counted) and t0 (end) are left as the original leaves them: the caller check
    // follows one call site (0x1fee78) into code that reads them.
    const uint32_t begin = g.read32(g.arg(0) + 8), end = g.read32(g.arg(0) + 0xC);
    int64_t seen = 0;
    if (begin != end)  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
        SET_GPR_S32(ctx, 8, end);  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
    for (uint32_t it = begin; it != end; it += 4)
    {
        const uint32_t obj = g.read32(it);
        if (static_cast<int64_t>(static_cast<int32_t>(g.read32(obj))) != g.sarg64(1))
            continue;
        if (seen == g.sarg64(2))
        {
            g.set_arg64(3, static_cast<uint64_t>(seen + 1));  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
            g.ret(obj);
            return;
        }
        ++seen;
    }
    g.set_arg64(3, static_cast<uint64_t>(seen));  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
    g.ret(0);
}
