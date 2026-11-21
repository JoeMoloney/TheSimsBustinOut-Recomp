// The number of set bits in the low 16 bits of a0 (the same code as Bits16_Count_001d27f0;
// not a byte-identical copy because the map joins other code after it). 1 call site.
#include "theseus.h"

void Bits16_Count_002e5330(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t value = static_cast<int16_t>(g.arg(0));
    uint32_t count = 0;
    for (int bit = 0; bit < 16; ++bit)
        count += (value >> bit) & 1;
    g.ret(count);
}
