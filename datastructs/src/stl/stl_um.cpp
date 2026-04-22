#include "stl_includes.h"
#ifdef STL_UM

#include <time.h>
#include <fstream>
#include <stdarg.h>
#include <format>
#include <iostream>

static std::ofstream logFile{};

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

void logging::LogOutputRawV(const char* fmt, va_list args) {
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

        if constexpr (LOG_TO_CONSOLE)
            printf(buff);
        if constexpr (LOG_TO_FILE)
			logFile.write(buff, size);

		free(buff);
    }
}

void logging::LogOutputRaw(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogOutputRawV(fmt, args);
    va_end(args);
}

void logging::LogOutput(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
    if (!logFile) return;

    const char* level_strings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};
    
    // Get current time
    time_t now = time(NULL);
    tm t{};

    localtime_s(&t, &now);
    
    // Print Header: [TIME] [LEVEL] [FILE:LINE in FUNC]
    std::string header = std::format(
        "[{:02}:{:02}:{:02}] [{}] [{}:{} in {}]: ",
        t.tm_hour, t.tm_min, t.tm_sec,
        level_strings[level], file, line, func
    );

    // log header
    LogOutputRaw(header.c_str());

    // log fmt
    va_list args;
    va_start(args, fmt);
    LogOutputRawV(fmt, args);
    va_end(args);
}

#endif
