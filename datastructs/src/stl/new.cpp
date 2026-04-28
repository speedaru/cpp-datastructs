#include "new.hpp"
#include "utils/logging.hpp"
#include <shared.h>

#ifdef STL_KM

void* operator new(size_t size) { return FP_ALLOC(POOL_FLAG_NON_PAGED, size, POOL_TAG); }
void operator delete(void* ptr) { FP_FREE(ptr, POOL_TAG); }

// Sized deletes (C++14) — MSVC emits these from scalar/vector deleting destructors
void  operator delete  (void* ptr, size_t)    noexcept { FP_FREE(ptr, POOL_TAG); }
void  operator delete[](void* ptr, size_t)    noexcept { FP_FREE(ptr, POOL_TAG); }

// Also add the plain (non-sized) overloads — MSVC sometimes emits both
void  operator delete[](void* ptr)            noexcept { FP_FREE(ptr, POOL_TAG); }

#endif
