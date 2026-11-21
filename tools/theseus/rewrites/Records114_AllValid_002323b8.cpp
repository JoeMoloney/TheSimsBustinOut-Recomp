// Checks up to [a0+0x4500] records of 0x114 bytes from a0 (end a0+0x4500): records whose first
// word is 0 are skipped; a record with type [r+8] < 2 (unsigned) fails; a type other than 2 counts
// one checked record and fails unless [r+4] is 6. Returns 0 on a failure, else 1 (count 0, the
// count used up, or the end reached; the end compare is signed). 1 call site.
#include "theseus.h"

void Records114_AllValid_002323b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t base = g.arg(0);
    const uint32_t end = base + 0x4500;
    int32_t remaining = static_cast<int32_t>(g.read32(base + 0x4500));
    SET_GPR_S32(ctx, 8, 2);  // t0 and a3 hold the constants 2 and 6 as in the original (a callee of a caller reads them); only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    if (remaining != 0)  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
        SET_GPR_S32(ctx, 7, 6);  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    if (remaining == 0)
    {
        g.ret(1);
        return;
    }
    for (uint32_t r = base;;)
    {
        if (g.read32(r) != 0)
        {
            const uint32_t type = g.read32(r + 8);
            if (type < 2)
            {
                g.ret(0);
                return;
            }
            if (type != 2)
            {
                --remaining;
                if (g.read32(r + 4) != 6)
                {
                    g.ret(0);
                    return;
                }
            }
        }
        r += 0x114;
        if (!(static_cast<int32_t>(r) < static_cast<int32_t>(end)) || remaining == 0)
            break;
    }
    g.ret(1);
}
