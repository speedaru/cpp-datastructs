#pragma once

#define STL_UM
//#define STL_KM

#ifdef STL_KM
	#ifndef _AMD64_
        #define _AMD64_ 
    #endif
	#define NOMINMAX

	#undef min
	#undef max

	#include <ntifs.h>
	
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
	enum LogLevel {
		LOG_TRACE,
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR
	};

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
	constexpr bool LOG_TIME = false; // date time
	constexpr bool LOG_FILENAME = false; // filename and line number
	constexpr bool LOG_FUNCTION = false; // function

	constexpr int SPACES_PER_INDENT = 2; // spaces per indentation level
	inline int g_IndentLevel = 0; // Use thread-local if in multi-threaded user mode
	inline bool g_pauseLog = false;

	#define LOG_T(...) LogOutput(logging::LOG_TRACE, __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)
	#define LOG_D(...) LogOutput(logging::LOG_DEBUG, __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)
	#define LOG_I(...) LogOutput(logging::LOG_INFO,  __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)
	#define LOG_W(...) LogOutput(logging::LOG_WARN,  __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)
	#define LOG_E(...) LogOutput(logging::LOG_ERROR, __RELATIVE_FILE__, __LINE__, __func__, __VA_ARGS__)

	#define LOG_OBJ(level, fmt, ...) LogOutput(level, __RELATIVE_FILE__, __LINE__, __func__, \
										"[%s:0x%p] " fmt, this->m_tag, this, ##__VA_ARGS__)
	#define LOG_OBJ_D(fmt, ...) LOG_OBJ(logging::LOG_DEBUG, fmt, ##__VA_ARGS__)
	#define LOG_OBJ_T(fmt, ...) LOG_OBJ(logging::LOG_DEBUG, fmt, ##__VA_ARGS__)

	class ScopedLog {
	public:
		ScopedLog(const char* func) : m_func(func) {
			LOG_I(">> %s\n", m_func);
			g_IndentLevel++;
		}
		~ScopedLog() {
			g_IndentLevel--;
			LOG_I("<< %s\n", m_func);
		}

	private:
		const char* m_func;
	};

	class ScopedPauseLog {
	public:
		ScopedPauseLog(bool* pauseLog) : m_pPauseLog(pauseLog), m_prevVal(false) {
			if (m_pPauseLog) {
				m_prevVal = *m_pPauseLog;
				*m_pPauseLog = true;
			}
		}

		~ScopedPauseLog() {
			if (m_pPauseLog) {
				*m_pPauseLog = m_prevVal;
			}
		}

	private:
		bool m_prevVal;
		bool* m_pPauseLog;
	};

	#define LOG_SCOPE() logging::ScopedLog _scope(__func__)
	#define LOG_PAUSE_SCOPED() logging::ScopedPauseLog _pauseLog(&logging::g_pauseLog)
	#define LOG_PAUSE() logging::g_pauseLog = true
	#define LOG_UNPAUSE() logging::g_pauseLog = false

	// should always add at the end to not mess up access modifiers
	#define ADD_CLASS_TAG				\
	protected:							\
		const char* m_tag = "Unnamed";	\
	public:								\
		void SetTag(const char* tag) { m_tag = tag; }
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

	logging::LOG_D("[-] FREE: 0x%p [%zu bytes] at %s:%d (allocated: %zu)\n",
		ptr, size, file, line, GetBytesAllocated());

#ifdef STL_KM
	ExFreePoolWithTag(base, poolTag);
#else
	free(base);
#endif
}
