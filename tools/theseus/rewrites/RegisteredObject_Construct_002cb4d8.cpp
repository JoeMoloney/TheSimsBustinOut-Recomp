// Constructor of a small object that registers itself in a global intrusive list.
// Replaces FUN_002cb4d8 (in the bstring2.cpp region; the class name is not known yet).
//   +0x0 next object (the previous list head)    +0x8 id, from a global counter
//   +0x4 zero                                     +0xC vtable 0x3E69A8 (slots: 0, 0, 0x2CB510, 0x2CB580)
// Globals, $gp-relative (.sbss): head of the list at gp-0x7764, next id at gp-0x7760.
// Memory accesses stay in the original's order (they could overlap if `self` were a global).
#include "theseus.h"

namespace
{
    constexpr uint32_t kVtable = 0x003E69A8;
    constexpr int32_t kListHead = -0x7764, kNextId = -0x7760;
}

void RegisteredObject_Construct_002cb4d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t self = g.arg(0);
    const uint32_t id = g.read32(g.gp() + kNextId);
    const uint32_t head = g.read32(g.gp() + kListHead);
    g.write32(self + 0x8, id);
    g.write32(self + 0xC, kVtable);
    g.write32(self + 0x0, head);
    g.write32(self + 0x4, 0);
    g.write32(g.gp() + kNextId, id + 1);
    g.write32(g.gp() + kListHead, self);
    g.ret_copy_of_arg(0);
}
