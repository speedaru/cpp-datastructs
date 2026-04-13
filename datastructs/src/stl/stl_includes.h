#pragma once
#include <cassert>
#include <memory>

// macros
#define __RELATIVE_FILE__ (__FILE__ + sizeof(PROJECT_DIR) - 1)


namespace spd {
	// allocations
	inline size_t g_totalAllocated = 0;
	inline size_t g_totalFreed = 0;

	inline size_t GetBytesAllocated() {
		assert(g_totalAllocated >= g_totalFreed);
		return g_totalAllocated - g_totalFreed;
	}

	template <typename T>
	T* TrackedAlloc(size_t count, const char* file, int line);
	void TrackedFree(void* ptr, const char* file, int line);

	template <typename T>
	inline T min(T a, T b) { return a < b ? a : b; }
	template <typename T>
	inline T max(T a, T b) { return a > b ? a : b; }
}

#define SPD_ALLOC(T, count) spd::TrackedAlloc<T>(count, __RELATIVE_FILE__, __LINE__)
#define SPD_FREE(ptr) spd::TrackedFree(ptr, __RELATIVE_FILE__, __LINE__)


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

	#define LOG_T(...) LogOutput(logging::LOG_TRACE, __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define LOG_D(...) LogOutput(logging::LOG_DEBUG, __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define LOG_I(...) LogOutput(logging::LOG_INFO,  __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define LOG_W(...) LogOutput(logging::LOG_WARN,  __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define LOG_E(...) LogOutput(logging::LOG_ERROR, __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
}


// ------------------------- IMPL -------------------------

template<typename T>
inline T* spd::TrackedAlloc(size_t count, const char* file, int line) {
	// allocate size for header + actual size
    size_t size = count * sizeof(T);
	T* ptr = static_cast<T*>(malloc(size));

	g_totalAllocated += size;
	
	LOG_D("ALLOC: 0x%p [%zu bytes] for %zu elements at %s:%d (allocated: %zu)\n",
		ptr, size, count, file, line, GetBytesAllocated());

	return ptr;
}

inline void spd::TrackedFree(void* ptr, const char* file, int line) {
	if (!ptr) {
		return;
	}

	size_t size = static_cast<size_t*>(ptr)[-2];

	g_totalFreed += size;

	logging::LOG_D("FREE: 0x%p [%zu bytes] at %s:%d (allocated: %zu)\n",
		ptr, size, file, line, GetBytesAllocated());

	free(ptr);
}
