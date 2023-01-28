// linux only (timestamp)

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>


// https://en.wikipedia.org/wiki/ANSI_escape_code
#define __ESC "\033"
#define __TERMINAL_DEFAULT __ESC "[m"
#define __RECORD_BUFSIZE (128)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

static bool get_timestamp(char* buffer, size_t size, size_t* offset);
static void __log(Severity severity, const char* filename, int line, const char* format, ...);

#define __lTrace(...)   __log(SEV_TRACE   , __FILENAME__, __LINE__,      NULL  )
#define __lDebug(...)   __log(SEV_DEBUG   , __FILENAME__, __LINE__, __VA_ARGS__)
#define __lInfo(...)    __log(SEV_INFO    , __FILENAME__, __LINE__, __VA_ARGS__)
#define __lWarn(...)    __log(SEV_WARNING , __FILENAME__, __LINE__, __VA_ARGS__)
#define __lError(...)   __log(SEV_ERROR   , __FILENAME__, __LINE__, __VA_ARGS__)

typedef enum Severity
{
    SEV_NONE = -1,
    SEV_TRACE,
    SEV_DEBUG,
    SEV_INFO,
    SEV_WARNING,
    SEV_ERROR,
    SEV_MAX = SEV_ERROR
} Severity;

const char* __severityNames[] =
{
    "TRACE",
    "DEBUG",
    " INFO",
    " WARN",
    "ERROR"
};

const char* __severityColor[] =
{
    __ESC "[38;5;247m",
    "",
    "",
    __ESC "[38;5;166m",
    __ESC "[38;5;160m",
};

int main()
{
    __lTrace();
    __lDebug("format %s", "debug");
    __lInfo("format %s", "info");
    __lWarn("format %s", "warning");
    __lError("format %s", "error");

    return 0;
}

bool get_timestamp(char* buffer, size_t size, size_t* offset)
{
    bool success = false;
    
    do
    {
        // get sec and nsec from epoch
        struct timespec tms;
        int result = clock_gettime(CLOCK_REALTIME, &tms);
        if (result == -1)
        {
            // errno
            break;
        }

        // convert sec to different intervals
        struct tm resultTime = { 0 };
        struct tm* pResultTime = localtime_r(&tms.tv_sec, &resultTime);
        if (pResultTime == NULL)
        {
            // errno
            break;
        }

        // format intervals
        char formatedTime[64] = { 0 };
        size_t written = strftime(buffer, size, "%m-%d %H:%M:%S.", pResultTime);
        if (written == 0)
            break;
        *offset += written;

        // convert nsec to msec and format
        uint64_t msec = ((uint64_t)(tms.tv_nsec)) / 1000000;
        result = snprintf(buffer + written, size - written, "%03zu", (uint64_t)msec);
        if (result < 0)
        {
            buffer[*offset] = 0;
            break;
        }
        *offset += result;

        success = true;
    } while (0);

    return success;
}

void __log(Severity severity, const char* filename, int line, const char* format, ...)
{
    char record[__RECORD_BUFSIZE] = { 0 };
    char* result = "record error\n";
    size_t offset = 0;

    do
    {
        // timestamp
        bool gotTimestamp = get_timestamp(record, __RECORD_BUFSIZE, &offset);
        if (!gotTimestamp)
            break;

        // severity and code location
        const char* color = __severityColor[severity];
        int written = snprintf(record + offset, __RECORD_BUFSIZE - offset, 
                               "  [ %s%s%s ]  %s:%-10d", 
                               color, __severityNames[severity], __TERMINAL_DEFAULT,
                               filename, line);
        offset += written;
        if (written < 0)
            break;

        // message
        if (format)
        {
            // message is not empty: write
            va_list args = { 0 };
            va_start(args, format);
            written = vsnprintf(record + offset, __RECORD_BUFSIZE - offset, format, args);
            va_end(args);
            if (written < 0)
                break;
        }

        result = record;
    } while (0);

    printf("%s\n", result);
}