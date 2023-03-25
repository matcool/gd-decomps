#pragma once

#include <cstdint>

using std::intptr_t;
using std::uintptr_t;

#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE [[gnu::always_inline]]
#endif

template <class T, class P>
inline ALWAYS_INLINE T& from(P* ptr, intptr_t offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) + offset);
}

// this->field\d+?_(0x[0-9a-fA-F]+)
// from<void>(this, $1)

static constexpr auto DECOMP_HOOK_PRIORITY = 99999;

#define SET_DECOMP_PRIORITY(name) \
    static void onModify(auto& self) { \
        (void) self.setHookPriorty(name, DECOMP_HOOK_PRIORITY); \
    }
