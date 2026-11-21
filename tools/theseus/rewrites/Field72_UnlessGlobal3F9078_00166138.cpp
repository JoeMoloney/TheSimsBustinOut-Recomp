// 0 if the global 0x3F9078 is non-zero, else the u16 at a0+0x72. 1 call site.
#include "theseus.h"

void Field72_UnlessGlobal3F9078_00166138(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(0x003F9078) != 0 ? 0 : g.read16(g.arg(0) + 0x72));
}
