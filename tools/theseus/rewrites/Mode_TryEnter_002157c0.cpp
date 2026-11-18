// Enter "mode" id (1..24) if no mode is active (25 callers). Replaces FUN_002157c0.
// Globals: active mode 0x3F92B8, plus 0x3F92B4 and 0x3F92BC which are reset on entry.
// Returns the id when entered, 0 otherwise. What the modes are is not known yet.
#include "theseus.h"

namespace
{
    constexpr uint32_t kActive = 0x003F92B8, kResetA = 0x003F92B4, kResetB = 0x003F92BC;
}

void Mode_TryEnter_002157c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t id = g.arg(0);
    if (g.read32(kActive) != 0 || id - 1 >= 24)
    {
        g.ret(0);
        return;
    }
    g.write32(kActive, id);  // gate: equivalent write16: only reached with the slot 0 and id <= 24, so the high half stays 0
    g.write32(kResetA, 0);
    g.write32(kResetB, 0);
    g.ret_copy_of_arg(0);
}
