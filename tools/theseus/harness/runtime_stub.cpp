// Stub PS2Runtime for the Theseus diff-test harness.
//
// The harness links one generated function (the original) and one rewrite against these
// definitions instead of the real runtime. Generated code reaches the runtime only through the
// members defined here; normal RAM accesses are inline (rdram[addr & PS2_RAM_MASK]).
// PS2Runtime has no virtual functions and none of these touch its data members, so the harness
// calls them through a pointer to raw storage (see difftest_harness.cpp).
//
// Everything a function does through the runtime is recorded in g_events, and the harness
// requires the original and the rewrite to produce the same events.
#include "ps2_runtime_macros.h"
#include "ps2_runtime.h"

#include "runtime_stub.h"

namespace theseus_stub
{
    Events g_events;

    namespace
    {
        void mix(uint64_t value)
        {
            // FNV-1a over 64-bit words: order-sensitive fingerprint of runtime traffic.
            g_events.hash = (g_events.hash ^ value) * 0x100000001B3ull;
        }

        uint64_t special_value(uint32_t addr, uint32_t size)
        {
            // Hardware registers read back a fixed pattern derived from the address, so both
            // versions see the same value and a read that moved is still noticed.
            return (0x9E3779B97F4A7C15ull * (addr | 1u)) ^ size;
        }
    }
}

using namespace theseus_stub;

bool PS2Runtime::dispatchGuestBranch(uint8_t *, R5900Context *ctx, uint32_t targetPc, uint32_t sourcePc,
                                     uint32_t, GuestBranchKind kind, const char *)
{
    // Same as the real runtime for a Return / jump (set pc, unwind). Calls are not followed:
    // leaf functions have none, and any call is recorded so it shows up as a difference.
    ctx->pc = targetPc;
    if (kind == GuestBranchKind::Return)
        ++g_events.returns;
    else
        ++g_events.transfers;
    mix(0xB0000000ull | static_cast<uint64_t>(kind));
    mix(targetPc);
    (void)sourcePc;  // differs between original and rewrite by design
    return false;
}

bool PS2Runtime::eeCheckpointDue(uint32_t) noexcept
{
    // Never yield: the function runs to completion in one call. (In the game a loop may hand
    // control back to the scheduler at a backward branch; that changes timing, not results.)
    return false;
}

#define THESEUS_LOAD(bits, type)                                                       \
    type PS2Runtime::Load##bits(uint8_t *, R5900Context *, uint32_t vaddr)             \
    {                                                                                   \
        ++g_events.special_loads;                                                       \
        mix((uint64_t(bits) << 32) | 0x10000000ull | vaddr);                            \
        return static_cast<type>(special_value(vaddr, bits));                           \
    }
THESEUS_LOAD(8, uint8_t)
THESEUS_LOAD(16, uint16_t)
THESEUS_LOAD(32, uint32_t)
THESEUS_LOAD(64, uint64_t)
#undef THESEUS_LOAD

__m128i PS2Runtime::Load128(uint8_t *, R5900Context *, uint32_t vaddr)
{
    ++g_events.special_loads;
    mix((uint64_t(128) << 32) | 0x10000000ull | vaddr);
    return _mm_set_epi64x(static_cast<int64_t>(special_value(vaddr, 129)),
                          static_cast<int64_t>(special_value(vaddr, 128)));
}

#define THESEUS_STORE(bits, type)                                                      \
    void PS2Runtime::Store##bits(uint8_t *, R5900Context *, uint32_t vaddr, type value) \
    {                                                                                   \
        ++g_events.special_stores;                                                      \
        mix((uint64_t(bits) << 32) | 0x20000000ull | vaddr);                            \
        mix(static_cast<uint64_t>(value));                                              \
    }
THESEUS_STORE(8, uint8_t)
THESEUS_STORE(16, uint16_t)
THESEUS_STORE(32, uint32_t)
THESEUS_STORE(64, uint64_t)
#undef THESEUS_STORE

void PS2Runtime::Store128(uint8_t *, R5900Context *, uint32_t vaddr, __m128i value)
{
    ++g_events.special_stores;
    mix((uint64_t(128) << 32) | 0x20000000ull | vaddr);
    mix(static_cast<uint64_t>(_mm_extract_epi64(value, 0)));
    mix(static_cast<uint64_t>(_mm_extract_epi64(value, 1)));
}

void PS2Runtime::handleSyscall(uint8_t *, R5900Context *ctx)
{
    ++g_events.syscalls;
    mix(0x30000000ull | GPR_U32(ctx, 3));
}

void PS2Runtime::handleSyscall(uint8_t *, R5900Context *, uint32_t id)
{
    ++g_events.syscalls;
    mix(0x30000000ull | id);
}

void PS2Runtime::handleBreak(uint8_t *, R5900Context *)
{
    ++g_events.breaks;
    mix(0x40000000ull);
}

void PS2Runtime::handleTrap(uint8_t *, R5900Context *)
{
    ++g_events.traps;
    mix(0x50000000ull);
}

void PS2Runtime::SignalException(R5900Context *, PS2Exception exception)
{
    ++g_events.exceptions;
    mix(0x60000000ull | static_cast<uint32_t>(exception));
}

void PS2Runtime::clearLLBit(R5900Context *ctx)
{
    ctx->llbit = 0;
}
