// For each of the [a0+0x157BC] objects in the pointer array at a0+0x12FBC (count re-read every step): clears bit 14 (0x4000) of the object'\''s +0x1C. Returns the last object, or the count when it is <= 0 (32-bit test, as the generated code does). 1 call site. Pairs with List_AppendValuesTo12FBC_00279218.
#include "theseus.h"

void List12FBC_ClearFlag4000_00279610(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t count_at = g.arg(0) + 0x157BC;
    const int32_t count = static_cast<int32_t>(g.read32(count_at));
    if (count <= 0)
    {
        g.ret(static_cast<uint32_t>(count));
        return;
    }
    uint32_t slot = g.arg(0) + 0x12FBC, obj = 0;
    int32_t i = 0;
    do
    {
        obj = g.read32(slot);
        ++i;
        g.write32(obj + 0x1C, g.read32(obj + 0x1C) & 0xFFFFBFFFu);  // gate: equivalent write16: only bit 14 changes
        slot += 4;
    } while (i < static_cast<int32_t>(g.read32(count_at)));
    SET_GPR_S32(ctx, 4, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(obj);
}
