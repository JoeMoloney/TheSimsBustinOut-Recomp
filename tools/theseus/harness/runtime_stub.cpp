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

#include "runtime/ee_native_calls.h"
#include "runtime_stub.h"

namespace theseus_stub
{
    Events g_events;
    const char *g_skip_reason = nullptr;
    const uint8_t *g_pristine = nullptr;

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

namespace
{
    uint64_t derive(uint64_t h, uint64_t salt)
    {
        uint64_t x = h ^ (salt * 0x9E3779B97F4A7C15ull);
        x ^= x >> 33; x *= 0xFF51AFD7ED558CCDull; x ^= x >> 33; x *= 0xC4CEB9FE1A85EC53ull; x ^= x >> 33;
        return x;
    }

    void put32(uint8_t *rdram, uint32_t addr, uint32_t v)
    {
        for (uint32_t k = 0; k < 4; ++k)
            rdram[(addr + k) & (PS2_RAM_SIZE - 1)] = uint8_t(v >> (8 * k));
    }

    // A called function, simulated identically for the original's calls (dispatchGuestBranch)
    // and the rewrite's (callGuest). The call itself (target, return address, a0..a7) goes into
    // the fingerprint; then, derived from the fingerprint so both versions see the same thing:
    //  - the callee writes a word through each pointer argument into the data window and at
    //    two more window spots (catches a rewrite that caches memory across a call)
    //  - v0 becomes 0, 1, a window pointer or a random value
    //  - the scratch registers and hi/lo are scrambled (catches reliance on them surviving)
    //  - pc = the return address
    void simulate_call(uint8_t *rdram, R5900Context *ctx, uint32_t target, uint32_t returnAddress)
    {
        ++g_events.transfers;
        mix(0xCA11000000000000ull | target);
        mix(returnAddress);
        for (int r = 4; r <= 11; ++r)
            mix(GPR_U64(ctx, r));
        const uint64_t h = g_events.hash;
        for (int r = 4; r <= 7; ++r)
        {
            const uint32_t p = GPR_U32(ctx, r);
            if (p >= kWindow && p <= kWindow + kWindowSize - 4)
                put32(rdram, p, uint32_t(derive(h, r)));
        }
        put32(rdram, kWindow + uint32_t(derive(h, 100) % (kWindowSize - 4)), uint32_t(derive(h, 101)));
        put32(rdram, kWindow + uint32_t(derive(h, 102) % (kWindowSize - 4)), uint32_t(derive(h, 103)));
        const uint64_t pick = derive(h, 1);
        uint64_t v0;
        switch (pick & 3)
        {
        case 0: v0 = 0; break;
        case 1: v0 = 1; break;
        case 2: v0 = kWindow + uint32_t((pick >> 8) % (kWindowSize / 2)); break;
        default: v0 = uint64_t(int64_t(int32_t(uint32_t(pick >> 32)))); break;
        }
        SET_GPR_U64(ctx, 2, v0);
        for (int r : {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 24, 25})
            SET_GPR_U64(ctx, r, derive(h, 200 + r));
        ctx->hi = derive(h, 300);
        ctx->lo = derive(h, 301);
        ctx->pc = returnAddress;
    }
}

bool PS2Runtime::dispatchGuestBranch(uint8_t *rdram, R5900Context *ctx, uint32_t targetPc, uint32_t sourcePc,
                                     uint32_t fallthroughPc, GuestBranchKind kind, const char *)
{
    (void)sourcePc;  // differs between original and rewrite by design
    switch (kind)
    {
    case GuestBranchKind::Return:
        ++g_events.returns;
        mix(0xB0000000ull | static_cast<uint64_t>(kind));
        mix(targetPc);
        ctx->pc = targetPc;
        return false;
    case GuestBranchKind::DirectCall:
    case GuestBranchKind::IndirectCall:
        simulate_call(rdram, ctx, targetPc, fallthroughPc);  // generated code set ra already
        return true;                                         // continue after the call
    default:
        // Tail jump: the callee returns straight to our caller.
        simulate_call(rdram, ctx, targetPc, GPR_U32(ctx, 31));
        return false;
    }
}

namespace ps2x::native
{
    void runOnFiber(PS2Runtime &runtime, PS2Runtime::RecompiledFunction fn, uint8_t *rdram, R5900Context *ctx)
    {
        fn(rdram, ctx, &runtime);  // no scheduler here: run directly
    }

    R5900Context *callGuest(PS2Runtime &, uint8_t *rdram, R5900Context *ctx, uint32_t target, uint32_t returnAddress)
    {
        SET_GPR_U32(ctx, 31, returnAddress);  // as the real callGuest (and a `jal`) does
        simulate_call(rdram, ctx, target, returnAddress);
        return ctx;
    }
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
