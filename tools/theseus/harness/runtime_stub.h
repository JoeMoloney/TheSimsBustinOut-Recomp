// Runtime traffic recorded by runtime_stub.cpp; the harness resets it before each run.
#pragma once
#include <cstdint>

namespace theseus_stub
{
    // Guest memory layout shared by the harness and the stub.
    constexpr uint32_t kWindow = 0x01000000, kWindowSize = 0x10000;

    struct Events
    {
        uint64_t hash = 0xCBF29CE484222325ull;  // fingerprint of everything below, in order
        long returns = 0;                       // Return dispatches
        long transfers = 0;                     // calls and tail jumps out of the function
        long special_loads = 0, special_stores = 0;  // hardware addresses (runtime Load/Store)
        long syscalls = 0, breaks = 0, traps = 0, exceptions = 0;
    };
    extern Events g_events;
    // Set by an implementation that refuses a case (the Unicorn runner: an input a real CPU
    // would fault on); the harness then counts the case as skipped and compares nothing.
    extern const char *g_skip_reason;
    // The case's input RAM, untouched (the harness runs the rewrite first): an implementation
    // that refuses a case restores every page it wrote from here, so RAM B equals RAM A again.
    extern const uint8_t *g_pristine;
}
