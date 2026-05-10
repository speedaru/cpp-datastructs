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
    const char* level_strings[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR" };
    
    // time
    if constexpr (LOG_TIME) {
		time_t now = time(NULL);
		tm t{};
		localtime_s(&t, &now);

        LogOutputRaw("[%02u:%02u:%02u] ", t.tm_hour, t.tm_min, t.tm_sec);
    }

    // log debug level indicator
    LogOutputRaw("[%s]", level_strings[level]);

    // filename, line number, function
    if constexpr (LOG_FILENAME || LOG_FUNCTION) {
        if constexpr (LOG_FILENAME && LOG_FUNCTION) // filename, line number and function
            LogOutputRaw(" [%s:%d in %s]", file, line, func);
        else if constexpr (!LOG_FILENAME && LOG_FUNCTION) // only function
            LogOutputRaw(" [in %s]", func);
        else if constexpr (LOG_FILENAME && !LOG_FUNCTION) // only filename and line number
            LogOutputRaw(" [%s:%d]", file, line);
    }

    // indent levels
    char spacesBuff[128]{ 0 };
    char* it = spacesBuff;

	*it++ = ' '; // 1 initial space after headers
    if (level == LOG_INFO || level == LOG_WARN) {
        *it++ = ' '; // add 1 extra space since only 4 chars instead of 5 for IFNO and WARN
    }

	const char* end = it + (g_IndentLevel * SPACES_PER_INDENT); // indent level * spaces per indent
    SPD_ASSERT(end < spacesBuff + sizeof(spacesBuff));
    while (it < end) {
		*it++ = ' ';
	}

    LogOutputRaw(spacesBuff);

    // actual message
    va_list args;
    va_start(args, fmt);
    LogOutputRawV(fmt, args);
    va_end(args);
}

#endif
