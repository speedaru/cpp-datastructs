#pragma once
#include "stl_includes.h"

#ifdef STL_KM

inline void* operator new(size_t, void* ptr) noexcept { return ptr; }
inline void* operator new[](size_t, void* ptr) noexcept { return ptr; }
inline void  operator delete(void*, void*) noexcept {}
inline void  operator delete[](void*, void*) noexcept {}

#endif