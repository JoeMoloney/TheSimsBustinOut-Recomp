// std::__adjust_heap(int *first = a0, int holeIndex = a1, int len = a2, int value = a3) with less<int> (SGI STL, __push_heap inlined): moves the hole down to a leaf along the larger children, then pushes value up from there no higher than holeIndex. Returns the address of the last store. The original compares indices and value as whole registers, so those are 64-bit here. 3 call sites.
#include "theseus.h"

void Heap_AdjustInt_0031d660(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t first = g.arg(0);
    const auto at = [&](int64_t i) { return first + (static_cast<uint32_t>(i) << 2); };
    const auto load = [&](int64_t i) { return static_cast<int64_t>(static_cast<int32_t>(g.read32(at(i)))); };
    const int64_t top = g.sarg64(1), len = g.sarg64(2), value = g.sarg64(3);
    int64_t hole = top;
    int64_t child = static_cast<int32_t>((static_cast<uint32_t>(top) << 1) + 2);
    while (child < len)
    {
        if (load(child) < load(child - 1))
            child = static_cast<int32_t>(static_cast<uint32_t>(child) - 1);
        g.write32(at(hole), g.read32(at(child)));
        hole = child;
        child = static_cast<int32_t>((static_cast<uint32_t>(child) + 1) << 1);
    }
    if (child == len)
    {
        g.write32(at(hole), g.read32(at(child) - 4));
        hole = static_cast<int32_t>(static_cast<uint32_t>(child) - 1);
    }
    // __push_heap: parent = (hole - 1) / 2, rounded toward zero
    int64_t parent = static_cast<int32_t>(static_cast<uint32_t>(hole) - 1) / 2;
    while (top < hole && load(parent) < value)
    {
        g.write32(at(hole), g.read32(at(parent)));
        hole = parent;
        parent = static_cast<int32_t>(static_cast<uint32_t>(hole) - 1) / 2;
    }
    g.write32(at(hole), static_cast<uint32_t>(value));
    g.ret(at(hole));
}
