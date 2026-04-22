#include "new.hpp"

#ifdef STL_KM

// Sized deletes (C++14) — MSVC emits these from scalar/vector deleting destructors
void  operator delete  (void* ptr, size_t)    noexcept { SPD_FREE(ptr); }
void  operator delete[](void* ptr, size_t)    noexcept { SPD_FREE(ptr); }

// Also add the plain (non-sized) overloads — MSVC sometimes emits both
void  operator delete  (void* ptr)            noexcept { SPD_FREE(ptr); }
void  operator delete[](void* ptr)            noexcept { SPD_FREE(ptr); }

#endif
