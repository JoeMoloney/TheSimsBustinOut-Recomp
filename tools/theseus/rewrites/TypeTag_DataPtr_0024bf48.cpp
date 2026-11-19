// Where the data of a tagged value lives. The word at a0 is a tag: low 15 bits = type,
// bit 15 = flag. With the flag set:
//   types 9, 10, 20..27   -> the data is inline: a0 + 4
//   types 12..18          -> the data is at +0xC of the object at +0x4C
//   type 19               -> the same, but 0 if that object pointer is null
// everything else -> 0. (8 call sites.)
#include "theseus.h"

void TypeTag_DataPtr_0024bf48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const uint32_t tag = g.read32(a);  // gate: equivalent read16: only bits 0-15 are used
    const uint32_t type = tag & 0x7FFF;
    const bool flag = (tag & 0x8000) != 0;
    const bool inline_data = (type >= 20 && type <= 27) || type == 9 || type == 10;
    if (flag && inline_data)
    {
        g.ret(a + 4);
        return;
    }
    if (!flag || type < 12 || type > 19)  // gate: equivalent 20: a flagged type 20 returned above
    {
        g.ret(0);
        return;
    }
    const uint32_t obj = g.read32(a + 0x4C);
    if (type == 19 && obj == 0)
    {
        g.ret(0);
        return;
    }
    g.ret(g.read32(obj + 0xC));
}
