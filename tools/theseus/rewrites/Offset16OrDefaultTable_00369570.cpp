// If the record at [a0+0xC] exists and its s16 at +4 is not -1: returns (int16)(that + a1).
// Otherwise, if [a0+4] is 0 and 0 <= a1 < 19 (all 64 bits): the s16 entry a1 of the table at
// 0x3F42E0; else 0. 2 call sites.
#include "theseus.h"

void Offset16OrDefaultTable_00369570(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t record = g.read32(g.arg(0) + 0xC);
    if (record != 0)
    {
        const uint16_t base = g.read16(record + 4);  // gate: equivalent read32: truncated to uint16_t
        if (base != 0xFFFF)
        {
            g.ret(static_cast<int16_t>(base + g.arg(1)));
            return;
        }
    }
    const int64_t index = g.sarg64(1);
    if (g.read32(g.arg(0) + 4) != 0 || index < 0 || index >= 19)
    {
        g.ret(0);
        return;
    }
    g.ret(g.sread16(0x003F42E0 + (g.arg(1) << 1)));
}
