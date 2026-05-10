#include "new.hpp"

#ifdef STL_KM

// regular new and delete

void* operator new(size_t size) { return SPD_ALLOC(unsigned char, size); }
void operator delete(void* ptr) { SPD_FREE(ptr); }

// Sized deletes (C++14) — MSVC emits these from scalar/vector deleting destructors

void  operator delete  (void* ptr, size_t)    noexcept { SPD_FREE(ptr); }
void  operator delete[](void* ptr, size_t)    noexcept { SPD_FREE(ptr); }

// Also add the plain (non-sized) overloads — MSVC sometis emits both

void  operator delete[](void* ptr)            noexcept { SPD_FREE(ptr); }

#endif
