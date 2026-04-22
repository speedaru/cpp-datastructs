#pragma once

//#define STL_UM
#define STL_KM

#ifdef STL_KM
	#ifndef _AMD64_
        #define _AMD64_ 
    #endif
	#define NOMINMAX

	#include <ntddk.h>
	
	#define SPD_ASSERT(cond) ASSERT(cond)
	#define SPD_POOL_TAG 'Spdl'
#else
	#include <cassert>
	#include <memory>

	#define SPD_ASSERT(cond) assert(cond)
#endif

// macros
#define __RELATIVE_FILE__ (__FILE__ + sizeof(PROJECT_DIR "src\\") - 1)

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

#ifdef STL_KM
	#define SPD_ALLOC(T, count) spd::TrackedAlloc<T>(count, __RELATIVE_FILE__, __LINE__, SPD_POOL_TAG)
	#define SPD_FREE(ptr) spd::TrackedFree(ptr, __RELATIVE_FILE__, __LINE__, SPD_POOL_TAG)
#else
	#define SPD_ALLOC(T, count) spd::TrackedAlloc<T>(count, __RELATIVE_FILE__, __LINE__)
	#define SPD_FREE(ptr) spd::TrackedFree(ptr, __RELATIVE_FILE__, __LINE__)
#endif


namespace logging {
	typedef enum {
		LOG_TRACE,
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR
	} LogLevel;

	// initialize logger (opens the file)
	void LoggerInit(const char* filename);
	// close logger
	void LoggerShutdown();
	// core logging function (don't call directly, use macros)
	void LogOutput(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...);
	void LogOutputRawV(const char* fmt, va_list args);
	void LogOutputRaw(const char* fmt, ...);

	constexpr bool LOG_TO_FILE = true;
	constexpr bool LOG_TO_CONSOLE = true;

	#define LOG_T(...) LogOutput(logging::LOG_TRACE, __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)
	#define LOG_D(...) LogOutput(logging::LOG_DEBUG, __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)
	#define LOG_I(...) LogOutput(logging::LOG_INFO,  __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)
	#define LOG_W(...) LogOutput(logging::LOG_WARN,  __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)
	#define LOG_E(...) LogOutput(logging::LOG_ERROR, __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)
}


// ------------------------- IMPL -------------------------

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
	
	LOG_D("ALLOC: 0x%p [%zu bytes] for %zu elements at %s:%d (allocated: %zu)\n",
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

	logging::LOG_D("FREE: 0x%p [%zu bytes] at %s:%d (allocated: %zu)\n",
		ptr, size, file, line, GetBytesAllocated());

#ifdef STL_KM
	ExFreePoolWithTag(base, poolTag);
#else
	free(base);
#endif
}
