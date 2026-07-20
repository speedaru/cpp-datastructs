#pragma once
#include "macros.h"
#include "logging.h"

// enable/disable custom allocation hooking
#define SPD_ENABLE_TRACKED_ALLOC 0

namespace spd {
	// allocations
	struct alignas(16) AllocHeader {
		size_t size;
	};

	inline size_t g_totalAllocated = 0;
	inline size_t g_totalFreed = 0;

	inline size_t GetBytesAllocated() {
		SPD_ASSERT(g_totalAllocated >= g_totalFreed);
		return g_totalAllocated - g_totalFreed;
	}

#ifdef STL_KM
	template <typename T>
	T* TrackedAlloc(size_t count, const char* file, int line, ULONG poolTag);
	void TrackedFree(void* ptr, const char* file, int line, ULONG poolTag);
#else
	template <typename T>
	T* TrackedAlloc(size_t count, const char* file, int line);
	void TrackedFree(void* ptr, const char* file, int line);
#endif

	template <typename T>
	inline T min(T a, T b) { return a < b ? a : b; }
	template <typename T>
	inline T max(T a, T b) { return a > b ? a : b; }
}

// macros

#if SPD_ENABLE_TRACKED_ALLOC
	#ifdef STL_KM
		#define SPD_ALLOC(T, count) spd::TrackedAlloc<T>(count, __RELATIVE_FILE__, __LINE__, SPD_POOL_TAG)
		#define SPD_FREE(ptr) spd::TrackedFree(ptr, __RELATIVE_FILE__, __LINE__, SPD_POOL_TAG)
	#else
		#define SPD_ALLOC(T, count) spd::TrackedAlloc<T>(count, __RELATIVE_FILE__, __LINE__)
		#define SPD_FREE(ptr) spd::TrackedFree(ptr, __RELATIVE_FILE__, __LINE__)
	#endif
#else
	// use standard allocators to avoid conflicts
	#define SPD_ALLOC(T, count) static_cast<T*>(malloc(sizeof(T) * count))
	#define SPD_FREE(ptr) free(ptr)
#endif


// impl

#ifdef STL_KM
	template<typename T>
	inline T* spd::TrackedAlloc(size_t count, const char* file, int line, ULONG poolTag) {
#else
	template<typename T>
	inline T* spd::TrackedAlloc(size_t count, const char* file, int line) {
#endif
	// allocate size for header + actual size
    size_t size = count * sizeof(T);
	size_t totalSize = size + sizeof(AllocHeader);

#ifdef STL_KM
	// allocate more for metadata
	AllocHeader* base = static_cast<AllocHeader*>(ExAllocatePool2(POOL_FLAG_NON_PAGED, totalSize, poolTag));
#else
	AllocHeader* base = static_cast<AllocHeader*>(malloc(totalSize)); // allocate more for metadata
#endif
	if (!base) {
		LOG_E("failed to allocate non paged pool with tag\n");
		return nullptr;
	}
	base->size = size; // store allocated size

	g_totalAllocated += size; // logical allocated size

	T* ptr = reinterpret_cast<T*>(base + 1);
	
	LOG_D("[+] ALLOC: 0x%p [%zu bytes] for %zu elements at %s:%d (allocated: %zu)\n",
		ptr, size, count, file, line, GetBytesAllocated());

	return ptr;
}

#ifdef STL_KM
	inline void spd::TrackedFree(void* ptr, const char* file, int line, ULONG poolTag) {
#else
	inline void spd::TrackedFree(void* ptr, const char* file, int line) {
#endif
	if (!ptr) {
		return;
	}

	AllocHeader* base = (AllocHeader*)ptr - 1;

	// get size from metadata
	size_t size = base->size;
	g_totalFreed += size;

	LOG_D("[-] FREE: 0x%p [%zu bytes] at %s:%d (allocated: %zu)\n",
		ptr, size, file, line, GetBytesAllocated());

#ifdef STL_KM
	ExFreePoolWithTag(base, poolTag);
#else
	free(base);
#endif
}
