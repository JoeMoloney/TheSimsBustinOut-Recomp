// Interval table at a1 (12-byte entries {lo, hi, value}, ended by lo == -1; unsigned compares, a0
// on all 64 bits): the value of the innermost entry with lo <= a0 < hi (a later entry replaces
// the best one when it lies within it: hi <= best.hi and lo >= best.lo), or 0 if none or a1 is
// null. Once a best entry exists, an entry whose lo is above a0 ends the search (the table is
// sorted by lo). 1 call site.
#include "theseus.h"

void IntervalTable_InnermostValue_0022ad68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const auto sext = [](uint32_t v) { return static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(v))); };
    const uint32_t table = g.arg(1);
    if (g.arg64(1) == 0)
    {
        g.ret(0);
        return;
    }
    const uint64_t key = g.arg64(0);
    int32_t best = -1;
    uint32_t entry = table;
    if (g.read32(entry) != 0xFFFFFFFF)  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    {
        SET_GPR_S32(ctx, 9, 0xFFFFFFFF);  // t1, t3: the original's -1 constants (a callee of the caller reads them); only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
        SET_GPR_S32(ctx, 11, 0xFFFFFFFF);  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    }
    int32_t i = 0;
    for (; g.read32(entry) != 0xFFFFFFFF; ++i, entry += 12)
    {
        const uint64_t lo = sext(g.read32(entry));
        if (key >= lo && key < sext(g.read32(entry + 4)))
        {
            const uint32_t b = table + static_cast<uint32_t>(best) * 12;
            if (best == -1 || (!(sext(g.read32(b + 4)) < sext(g.read32(entry + 4))) && !(lo < sext(g.read32(b)))))
                best = i;
        }
        else if (best >= 0 && key < lo)
            break;
    }
    SET_GPR_S32(ctx, 10, static_cast<uint32_t>(i));  // t2: the entry index where the walk stopped; only a scratch register, kept for the caller check (not in the ABI compare); gate: equivalent
    g.ret(best == -1 ? 0 : g.read32(table + static_cast<uint32_t>(best) * 12 + 8));
}
