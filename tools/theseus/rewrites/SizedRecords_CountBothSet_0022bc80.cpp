// Walks variable-size records from a0 (record: +0 size in bytes, the next record at +4 + size;
// a size of 0 ends the walk) and counts those whose words at +4 and +8 are both non-zero. 1 call site.
#include "theseus.h"

void SizedRecords_CountBothSet_0022bc80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t record = g.arg(0), count = 0;
    uint32_t size = g.read32(record);
    while (size != 0)
    {
        if (g.read32(record + 4) != 0 && g.read32(record + 8) != 0)
            ++count;
        record += size + 4;
        size = g.read32(record);
    }
    g.ret(count);
}
