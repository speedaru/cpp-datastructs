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
    const char* level_strings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};
    
	// get kernel system time
    LARGE_INTEGER systemTime;
    LARGE_INTEGER localTime;
    TIME_FIELDS timeFields;

    KeQuerySystemTimePrecise(&systemTime); 
    ExSystemTimeToLocalTime(&systemTime, &localTime);
    RtlTimeToTimeFields(&localTime, &timeFields);

    // format header manually
	char header[256];
    RtlStringCchPrintfA(
        header, sizeof(header),
        "[%02u:%02u:%02u] [%s] [%s:%d in %s]: ",
        timeFields.Hour, timeFields.Minute, timeFields.Second,
        level_strings[level], file, line, func
    );

    LogOutputRaw(header);

    va_list args;
    va_start(args, fmt);
    LogOutputRawV(fmt, args);
    va_end(args);
}

#endif
