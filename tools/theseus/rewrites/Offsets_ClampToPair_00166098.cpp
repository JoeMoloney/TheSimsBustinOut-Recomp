// Clamps the two signed bytes +0x17/+0x18 of the object [0x3FA628] to -25..25 (written back, both, only if either changed) and stores them as words in the object [0x3F8254]: +0x20 = the second, +0x1C = the first. Returns that object's address. 4 call sites. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void Offsets_ClampToPair_00166098(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const auto clamp = [](int32_t v, bool &changed) {
        if (v < -25) { changed = true; return -25; }  // gate: equivalent <=: -25 clamps to itself; the write-back stores the same bytes
        if (v > 25) { changed = true; return 25; }  // gate: equivalent >=: 25 clamps to itself; the write-back stores the same bytes
        return v;
    };
    bool changed = false;
    const int32_t first = clamp(g.sread8(g.read32(0x3FA628) + 0x17), changed);
    const int32_t second = clamp(g.sread8(g.read32(0x3FA628) + 0x18), changed);
    if (changed)
    {
        g.write8(g.read32(0x3FA628) + 0x17, static_cast<uint8_t>(first));  // gate: equivalent write16: +0x18 is stored next
        g.write8(g.read32(0x3FA628) + 0x18, static_cast<uint8_t>(second));
    }
    const uint32_t out = g.read32(0x3F8254);
    g.write32(out + 0x20, static_cast<uint32_t>(second));
    g.write32(out + 0x1C, static_cast<uint32_t>(first));
    g.ret(out);
}
