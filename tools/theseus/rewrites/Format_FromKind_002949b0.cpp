// Maps a kind a1 to a format code *a2 and size *a3: 0 -> 0x3A if a0+0x368 == 1 else 0x32, size 2; 1 or -1 -> 0x31, size 4; 2 -> 0x30, size 4; other kinds store nothing. Returns the code (or what v0 holds: 2 for kind 0, 0x30/0x31 for unknown positive/negative kinds). The sign test follows the generated code (low 32 bits). 1 call site.
#include "theseus.h"

void Format_FromKind_002949b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t kind = g.sarg64(1);
    if (kind == 0)
    {
        g.write32(g.arg(2), g.read32(g.arg(0) + 0x368) == 1 ? 0x3A : 0x32);
        g.write32(g.arg(3), 2);
        g.ret(2);
        return;
    }
    uint32_t code;
    if (static_cast<int32_t>(g.arg(1)) > 0)
    {
        if (kind == 1)
            code = 0x31;
        else if (kind == 2)
            code = 0x30;
        else
        {
            g.ret(0x30);
            return;
        }
    }
    else if (kind == -1)
        code = 0x31;
    else
    {
        g.ret(0x31);
        return;
    }
    g.write32(g.arg(2), code);
    g.write32(g.arg(3), 4);
    SET_GPR_S32(ctx, 3, 4);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(code);
}
