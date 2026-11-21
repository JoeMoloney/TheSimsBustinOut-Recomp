// Case-insensitive strcmp: a character whose ctype entry (table at 0x3F6741, indexed by the signed
// byte) has bit 0 set (upper case) is compared as c + 0x20; returns the first non-zero difference,
// or 0 at the end of the first string. 1 call site.
#include "theseus.h"

void Str_CompareNoCase_00228028(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t p = g.arg(0), q = g.arg(1);
    for (;; ++p, ++q)
    {
        const int8_t c1 = g.sread8(p), c2 = g.sread8(q);  // gate: equivalent read8: stored in an int8_t
        const int32_t l1 = (g.read8(0x003F6741 + c1) & 1) ? c1 + 0x20 : c1;  // gate: equivalent read16: only bit 0 is used
        const int32_t l2 = (g.read8(0x003F6741 + c2) & 1) ? c2 + 0x20 : c2;  // gate: equivalent read16: only bit 0 is used
        if (l1 != l2 || c1 == 0)
        {
            // t0 = the last character of a0, t1 = the position in a1 (advanced past the end when the
            // strings are equal; the first position is a1's whole register): a callee of the caller reads them
            SET_GPR_S32(ctx, 8, c1);  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
            const uint32_t t1 = l1 != l2 ? q : q + 1;  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
            if (t1 == g.arg(1))  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
                SET_GPR_U64(ctx, 9, g.arg64(1));  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
            else
                SET_GPR_S32(ctx, 9, t1);  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
            g.ret(static_cast<uint32_t>(l1 - l2));
            return;
        }
    }
}
