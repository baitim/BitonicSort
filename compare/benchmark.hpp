#pragma once

#include <chrono>

#ifdef _MSC_VER
#define ASM asm
#else
#define ASM __asm
#endif

#define NO_OPT(x) ASM volatile("" : "+g" (x)) // simple callq with one jmpq

template <typename Func, typename... Args>
inline long double get_time(Func func, Args&&... args) {
    auto time_begin = std::chrono::high_resolution_clock::now();

    std::invoke(func, std::forward<Args>(args)...);

    auto time_end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<long double>(time_end - time_begin).count();
}