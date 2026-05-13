#include "stl_includes.h"
#ifdef STL_KM

#include <ntstrsafe.h>

// stack buffer for log messages to avoid frequent pool allocations
#define LOG_BUFFER_SIZE 512

void logging::LoggerInit(const char* filename) {
    UNREFERENCED_PARAMETER(filename);
}

void logging::LoggerShutdown() {
}

void logging::LogOutputRawV(const char* fmt, va_list args) {
	char buff[LOG_BUFFER_SIZE];

    // Kernel equivalent of vsnprintf
    NTSTATUS status = RtlStringCchVPrintfA(buff, LOG_BUFFER_SIZE, fmt, args);

    if (NT_SUCCESS(status)) {
        // DPFLTR_IHVDRIVER_ID allows you to filter these logs in WinDbg
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%s", buff);
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
    
    // dont log if logging paused
    if (g_pauseLog) {
        return;
    }

    // time
    if constexpr (LOG_TIME) {
		// get kernel system time
		LARGE_INTEGER systemTime;
		LARGE_INTEGER localTime;
		TIME_FIELDS timeFields;

		KeQuerySystemTimePrecise(&systemTime); 
		ExSystemTimeToLocalTime(&systemTime, &localTime);
		RtlTimeToTimeFields(&localTime, &timeFields);

        LogOutputRaw("[%02u:%02u:%02u] ", timeFields.Hour, timeFields.Minute, timeFields.Second);
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
