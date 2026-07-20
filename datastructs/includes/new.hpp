#pragma once
#include "macros.h"

#ifdef STL_KM

// placement new and placement delete

inline void* operator new(size_t, void* ptr) noexcept { return ptr; }
inline void* operator new[](size_t, void* ptr) noexcept { return ptr; }
inline void  operator delete(void*, void*) noexcept {}
inline void  operator delete[](void*, void*) noexcept {}

#endif