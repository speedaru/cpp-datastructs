#include "stl_includes.h"
#include <time.h>
#include <fstream>
#include <stdarg.h>
#include <format>

static std::ofstream logFile{};


//// allocations
//void* operator new(size_t size) {
//	g_totalAllocated += size;
//    void* ptr = malloc(size);
//    printf("allocated %llu bytes : 0x%p\n", size, ptr);
//	return ptr;
//}
//
//void operator delete(void* ptr) noexcept {
//    assert(ptr != nullptr);
//    printf("freed %u bytes : 0x%p\n", (uint32_t)((size_t*)ptr)[-2], ptr);
//	free(ptr);
//}
//
//void* operator new[](size_t size) { return ::operator new(size); }
//void operator delete[](void* ptr) noexcept { ::operator delete(ptr); }


void logging::LoggerInit(const char* filename) {
    logFile = std::ofstream(filename, std::ios::binary);
    if (!logFile) {
        fprintf(stderr, "failed to open file '%s'\n", filename);
        return;
    }
}

void logging::LoggerShutdown() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void logging::LogOutput(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
    if (!logFile) return;

    const char* level_strings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};
    
    // Get current time
    time_t now = time(NULL);
    tm t{};

    localtime_s(&t, &now);
    
    // Print Header: [TIME] [LEVEL] [FILE:LINE in FUNC]
	logFile << std::format(
		"[{:02}:{:02}:{:02}] [{}] [{}:{} in {}]: ",
		t.tm_hour, t.tm_min, t.tm_sec,
		level_strings[level], file, line, func
	);

    va_list args;
    va_start(args, fmt);

    // create a copy of args to calc size
	va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy); // Always end the copy

    if (size < 0) {
        va_end(args);
        return;
    }

    char* buff = (char*)malloc(size + 1);
    if (buff) {
		vsnprintf(buff, (size_t)size + 1, fmt, args);
		logFile.write(buff, size);
		free(buff);
    }

	va_end(args);
	logFile.flush();
}
