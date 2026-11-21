// wcsncpy for 16-bit characters: copies the string at a1 to a0, at most a2 characters, then
// pads with zero characters up to a2 in all. Returns a0. 5 call sites (2 identical copies).
// theseus: copies 0x349c70
#include "theseus.h"

void Wstr16_NCopy_0029d6a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t dst = g.arg(0), src = g.arg(1), left = g.arg(2);
    const bool none = g.arg64(2) == 0;  // the first two tests compare the whole register
    uint16_t c = g.read16(src);  // gate: equivalent read32: kept as a halfword
    if (c != 0)
    {
        if (none)
        {
            g.ret_copy_of_arg(0);
            return;
        }
        g.write16(dst, c);
        for (;;)
        {
            src += 2;
            c = g.read16(src);  // gate: equivalent read32: kept as a halfword
            dst += 2;
            --left;
            if (c == 0 || left == 0)
                break;
            g.write16(dst, c);
        }
        if (left == 0)
        {
            g.ret_copy_of_arg(0);
            return;
        }
    }
    else if (none)
    {
        g.ret_copy_of_arg(0);
        return;
    }
    do
    {
        g.write16(dst, 0);
        dst += 2;
    } while (--left != 0);
    g.ret_copy_of_arg(0);
}
