// input string format:  %<alignment>:<color>:<unit>{<extendedOption>}
//      note 1. <alignment>, <color> and <extendedOption >might be skipped
//      note 2. drefault values would be applied instead

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>

#ifdef __linux__
    #include <unistd.h>
    #include <pthread.h>
    #include <errno.h>
#elif defined(_WIN32)
    #include <Windows.h>
    #include <io.h>
    #define write _write
    #define fileno _fileno
#endif


///////////////////////////////////////////////////////////////////////////

// #define _lDEBUG
#ifdef _lDEBUG
    #define _S(string) printf("%s\n", string)
    #define _C(symbol) printf("char=%c", symbol)
    #define _RESULT                                                                 \
            printf("  result = %d (%d)\n", result, abs(fmtNode->alignment));        \
            printf("  bufPosition = '%s'\n", bufPosition);                          \
            printf("  buffer = '%s'\n", buffer);
#else 
    #define _S(string)
    #define _C(symbol)
    #define _RESULT 
#endif

// TODO:
//      struct handlers for stdout and file
//      possibility to change format in process (roll and reopen)
//      windows 
//          time
//          thread



///////////////////////////////////////////////////////////////////////////


/************************************************************************
 *                              M A C R O S                             *
 ************************************************************************/

// constants
#define LOG_RECORD_MAX_SIZE     (256)

// format triggers
#define FMT_UNIT_FIRST          '%' 
#define FMT_UNIT_LAST           ' ' 
#define FMT_EXT_OPT_FIRST       '{'
#define FMT_EXT_OPT_LAST        '}'
#define FMT_END_STRING          '\0'

// format help macros
#define FMT_BUFF_SIZE           (64)
#define FMT_UNIT_START_LEN      (1)     // lenght of FMT_UNIT_FIRST
#define FMT_UNIT_END_LEN        (1)     // length of FMT_UNIT_LAST
#define FMT_UNIT_CTRL_LEN       (FMT_UNIT_START_LEN + FMT_UNIT_END_LEN)
#define FMT_EXT_OPT_FIRST_STR   "{"     // nust be align with FMT_EXT_OPT_FIRST
#define FMT_ALIGN_DEFAULT       (0)
#define FMT_MS_SYMBOL           'f'
#define FMT_UNIT_MAX_SEPARS     (2)
#define FMT_UNIT_SEPAR          ':'

// format break helpers
#define FMT_ERROR_IF_FALSE(statement)           \
    if (statement == false)                     \
    {                                           \
        fmt_parser_stop_with_error(parser);     \
        break;                                  \
    }

#define FMT_ERROR_IF_TRUE(statement)            \
    if (statement == true)                      \
    {                                           \
        fmt_parser_stop_with_error(parser);     \
        break;                                  \
    }

// code location
#define __FILENAME_FORWARDSLASH__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define __FILENAME__ (strrchr(__FILENAME_FORWARDSLASH__, '\\') ? strrchr(__FILENAME_FORWARDSLASH__, '\\') + 1 : __FILENAME_FORWARDSLASH__)

// severity macros
#define ERR(...)        write_log(LOG_SEVERITY_ERROR_E,  __FILENAME__, __LINE__, __VA_ARGS__)	
#define WARN(...)       write_log(LOG_SEVERITY_WARN_E,   __FILENAME__, __LINE__, __VA_ARGS__)	
#define INFO(...)       write_log(LOG_SEVERITY_INFO_E,   __FILENAME__, __LINE__, __VA_ARGS__)	
#define DEBUG(...)      write_log(LOG_SEVERITY_DEBUG_E,  __FILENAME__, __LINE__, __VA_ARGS__)	
#define TRACE(...)      write_log(LOG_SEVERITY_TRACE_E,  __FILENAME__, __LINE__, __VA_ARGS__)	


/************************************************************************
 *                               T Y P E S                              *
 ************************************************************************/

typedef enum FmtParserSignalE
{
    FMT_SIGNAL_NONE,
    FMT_SIGNAL_UNIT_FIRST,
    FMT_SIGNAL_EXT_OPT_FIRST,
    FMT_SIGNAL_EXT_OPT_LAST,
    FMT_SIGNAL_UNIT_LAST,
    FMT_SIGNAL_END_STRING,
    FMT_SIGNAL_REGULAR
} FmtParserSignalEnum;

typedef enum FmtParserStateE
{    
    FMT_PARSE_GAP,
    FMT_PARSE_UNIT,
    FMT_PARSE_EXT_OPT,
    FMT_END
} FmtParserStateEnum;

typedef enum FmtUnitsE
{
    FMT_FILENAME_E,
    FMT_LINE_E,
    FMT_THREAD_E,
    FMT_SEVERITY_E,
    FMT_TIMESTAMP_E,
    FMT_MESSAGE_E,
    FMT_ENDLINE_E,
    FMT_UNIT_MAX_E
} FmtUnitsEnum;

const char* fmtUnitMnemonics[FMT_UNIT_MAX_E] =
{
    [FMT_FILENAME_E]  = "filename"  ,
    [FMT_LINE_E]      = "line"      ,
    [FMT_THREAD_E]    = "thread"    ,
    [FMT_SEVERITY_E]  = "severity"  ,
    [FMT_TIMESTAMP_E] = "timestamp" ,
    [FMT_MESSAGE_E]   = "message"   ,
    [FMT_ENDLINE_E]   = "endl"      , 
};

typedef struct FmtUnitNodeS
{
    FmtUnitsEnum unit;
    long alignment;
    char* color;
    char extOption[FMT_BUFF_SIZE];
    char gap[FMT_BUFF_SIZE];
    struct FmtUnitNodeS* next;
} FmtUnitNode;

typedef bool (*parserCallback)(void* arg);

typedef struct FmtParserS
{
    int output;
    bool parsed;
    parserCallback callback;

    FmtParserStateEnum currentState;
    const char* string;
    size_t index;

    char currentBuff[FMT_BUFF_SIZE];        
    char accumulateBuff[FMT_BUFF_SIZE];
    size_t currentIndex;      
    size_t accumulateIndex;

    FmtUnitsEnum unit;
    long align;
    char* color;
    char extOption[FMT_BUFF_SIZE];
} FmtParser;

typedef enum LogOutputIdE
{
	LOG_OUTPUT_ID_STDOUT_E = 0,
	LOG_OUTPUT_ID_FILE_E,
	LOG_OUTPUT_ID_MAX_E,
} LogOutputIdEnum;

typedef enum LogSeverityE
{
	LOG_SEVERITY_ERROR_E = 0,
	LOG_SEVERITY_WARN_E,
	LOG_SEVERITY_INFO_E,
	LOG_SEVERITY_DEBUG_E,
	LOG_SEVERITY_TRACE_E,
	LOG_SEVERITY_MAX_E,
} LogSeverityEnum;

const char* severitiesString[LOG_SEVERITY_MAX_E] = 
{
    "ERROR",
    " WARN",
    " INFO",
    "DEBUG",
    "TRACE"
};

/************************************************************************
 *					P R I V A T E   D A T A								*
 ************************************************************************/

FmtUnitNode* outFormats[LOG_OUTPUT_ID_MAX_E] = 
{
    [LOG_OUTPUT_ID_STDOUT_E] = NULL,
    [LOG_OUTPUT_ID_FILE_E] = NULL
};


/************************************************************************
 *                  F U N C T I O N S   P R O T O T Y P E S             *
 ************************************************************************/

// public functions
bool logging_set_format(LogOutputIdEnum output, const char* format);
void logging_destroy();
void write_log(LogSeverityEnum severity, const char* file, int line, const char* fmt, ...);

// log record functions
static size_t log_record_format(LogOutputIdEnum output, char* buffer, size_t buffSize, LogSeverityEnum severity, const char* file, int line, const char* fmt, va_list args);
static void log_record_write(LogOutputIdEnum output, const char* record, size_t recordLen);

// node list funtions
static bool get_timestamp(char* format, char* buffer, size_t bufSize, size_t* written);
static void fmt_push_single_node(FmtUnitNode* node, LogOutputIdEnum output);
static bool fmt_push_nodes(void* arg);

// fmt parser functions
static FmtUnitsEnum fmt_find_unit(const char* string, size_t length);
static bool fmt_verify_timestamp_option(const char* format);
static bool fmt_verify_extended_option(FmtUnitsEnum unit, bool isExtended, const char* option);
static bool fmt_parse_unit(FmtParser* parser, bool lastExists);
static bool fmt_handle_unit(FmtParser* parser);
static void fmt_parser_change_state(FmtParser* parser, FmtParserStateEnum newState);
static void fmt_parser_stop_with_error(FmtParser* parser);
static bool fmt_parser_flush_current(FmtParser* parser);
static bool fmt_parser_write_char(FmtParser* parser);
static void fmt_parser_clear_buffers(FmtParser* parser);
static void fmt_parser_handle_signal(FmtParser* parser, FmtParserSignalEnum signal);
static void fmt_parse_format(FmtParser* parser);        


/************************************************************************
 *					P U B L I C   F U N C T I O N S     				*
 ************************************************************************/

bool logging_set_format(LogOutputIdEnum output, const char* format)
{
    bool result = false;

    do
    {
        if ((output != LOG_OUTPUT_ID_STDOUT_E) && (output != LOG_OUTPUT_ID_FILE_E))
        {
            break;
        }

        if (!format || !strlen(format))
        {
            break;
        }

        FmtParser parser = 
        {
            .output = output,
            .parsed = false,
            .callback = fmt_push_nodes,
            .currentState = FMT_PARSE_GAP,
            .string = format,
            .index = 0,
            .currentBuff = { 0 },
            .accumulateBuff = { 0 },
            .currentIndex = 0,      
            .accumulateIndex = 0,
            .unit = FMT_UNIT_MAX_E,
            .align = 0,
            .color = NULL,
            .extOption = { 0 }
        };
        
        fmt_parse_format(&parser);
        result = parser.parsed;

    } while (0);

    if (!result)
    {
        // some nodes might be created befor failure
        logging_destroy();
    }

    return result;
}

void logging_destroy()
{
    for (int output = 0; output < 2; ++output)
    {
        FmtUnitNode* curr = outFormats[output];
        FmtUnitNode* temp;

        while (curr != NULL)
        {
            if (curr->color != NULL)
            {
                free(curr->color);
            }
            
            temp = curr->next;
            free(curr);
            curr = temp;
        };

        outFormats[output] = NULL;
    }
}

void write_log(LogSeverityEnum severity, const char* file, int line, const char* fmt, ...)
{
    va_list args = { 0 };
	va_start(args, fmt);

    for (LogOutputIdEnum output = LOG_OUTPUT_ID_STDOUT_E; output < LOG_OUTPUT_ID_MAX_E; ++output)
    {
        char record[LOG_RECORD_MAX_SIZE] = { 0 };
        size_t formatted = log_record_format(output, record, sizeof(record) - 1, severity, file, line, fmt, args);
        if (formatted)
        {
            log_record_write(output, record, formatted);
        }
    }

    va_end(args);
}

/************************************************************************
 *					S T A T I C   F U N C T I O N S     				*
 ************************************************************************/

size_t log_record_format(LogOutputIdEnum output, char* buffer, size_t buffSize, LogSeverityEnum severity, const char* file, int line, const char* fmt, va_list args)
{
    FmtUnitNode* fmtNode = outFormats[output];
    size_t written = 0;
    size_t sizeAvailable = buffSize;
    
    while (fmtNode != NULL)
    {
        // required for all units
        char* bufPosition = buffer + written;
        sizeAvailable = buffSize - written;
        int result = 0;
        
        // create format string
        char unitFmt[32] = { 0 };
        const char* color = fmtNode->color ? fmtNode->color : "";
        const char* reset = fmtNode->color ? "\033[m" : "";
        result = snprintf(unitFmt, sizeof(unitFmt), "%s%c%li%c%s", color, '%', fmtNode->alignment, 's', reset);
        if (result == -1)
            continue;

        switch (fmtNode->unit)
        {
            case FMT_UNIT_MAX_E:
            {
                _S("FMT_UNIT_MAX_E");

                result = snprintf(bufPosition, sizeAvailable, "%s", fmtNode->gap);           
                if (result != -1)
                    written += result;
            }
            break;

            case FMT_FILENAME_E:
            {
                _S("FMT_FILENAME_E");

                result = snprintf(bufPosition, sizeAvailable, unitFmt, file);
                if (result != -1)
                    written += result;
            }
            break;

            case FMT_LINE_E:
            {
                _S("FMT_LINE_E");

                char lineString[8] = { 0 };
                (void) snprintf(lineString, sizeof(lineString), "%i", line);
                result = snprintf(bufPosition, sizeAvailable, unitFmt, lineString);
                if (result != -1)
                    written += result;
            }
            break;            

            case FMT_THREAD_E:
            {
                _S("FMT_THREAD_E");

            #ifdef __linux__
                pthread_t thread = pthread_self();
            #elif defined(_WIN32)
                DWORD thread = GetCurrentThreadId();
            #endif
            
                char threadString[11] = { 0 };
                (void) snprintf(threadString, sizeof(threadString), "0x%08lx", thread);
                result = snprintf(bufPosition, sizeAvailable, unitFmt, threadString);
                if (result != -1)
                    written += result;
            }
            break;

            case FMT_SEVERITY_E:
            {    
                _S("FMT_SEVERITY_E");
                
                result = snprintf(bufPosition, sizeAvailable, unitFmt, severitiesString[severity]);
                if (result != -1)
                    written += result;
            }
            break;

            case FMT_TIMESTAMP_E:
            {
                _S("FMT_TIMESTAMP_E");
                
                char tsString[32] = { 0 };
                size_t offset = 0;
                bool gotTs = get_timestamp(fmtNode->extOption, tsString, sizeof(tsString), &offset);
                if (gotTs)
                {
                    result = snprintf(bufPosition, sizeAvailable, unitFmt, tsString);
                    if (result != -1)
                        written += result;
                }
            }
            break;

            case FMT_MESSAGE_E:
            {
                _S("FMT_MESSAGE_E");
                
                result = vsnprintf(bufPosition, sizeAvailable, fmt, args);
                if (result != -1)
                    written += result;
            }
            break;    

            case FMT_ENDLINE_E:
            {
                _S("FMT_ENDLINE_E");
                
                if (sizeAvailable > 1)
                {
                    *bufPosition = '\n';
                    ++written;
                }
            }
            break;   

            default:
            {
                _S("DEFAUILT_INVALID");
                
                // invalid unit
                break;
            }
        }

        fmtNode = fmtNode->next;
    }
    
    return written;
}

void log_record_write(LogOutputIdEnum output, const char* record, size_t recordLen)
{
    switch (output)
    {
        case LOG_OUTPUT_ID_STDOUT_E:
        {
            write(STDOUT_FILENO, record, recordLen);
        }
        break;

        case LOG_OUTPUT_ID_FILE_E:
        {
            // debug
            printf("... Writing to the FILE ...");
        }
        break;

        default:
        {
            // invalid output
        }
        break;
    }
}

bool get_timestamp(char* format, char* buffer, size_t bufSize, size_t* written)
{
    bool result = false;

    do
    {
    #ifdef __linux__
        struct timespec tms;
        clock_gettime(CLOCK_REALTIME, &tms);
        uint64_t rawTime = (uint64_t)tms.tv_sec;
        uint64_t msPart = ((uint64_t)tms.tv_nsec) / 1000000;
    #elif defined (_WIN32)
        struct _timeb timebuffer;
        _ftime(&timebuffer);
        uint64_t rawTime = (uint64_t)timebuffer.time;
        uint64_t msPart = timebuffer.millitm;
    #endif

        // determine whether ms part should be processed
        bool msRequired = false;
        size_t lastIndex = strlen(format);
        if (lastIndex >= 2)
        {
            if ((format[lastIndex - 2] == FMT_UNIT_FIRST) && (format[lastIndex - 1] == FMT_MS_SYMBOL))
            {
                msRequired = true;

                // remove %f so that strftime() may process
                format[lastIndex - 2] = 0;
                format[lastIndex - 1] = 0;
            }
        }

        // write main part
        struct tm* timeStruct = localtime(&rawTime);
        size_t writtenMain = strftime(buffer, bufSize, format, timeStruct);
        if (writtenMain <= 0)
        {
            memset(buffer, 0, bufSize);
            break;
        }

        // write ms
        int writtenMs = 0;
        if (msRequired)
        {
            // restore %f
            format[lastIndex - 2] = '%';
            format[lastIndex - 1] = 'f';
            
            char* dest = buffer + writtenMain;
            size_t size = bufSize - writtenMain;

            writtenMs = snprintf(dest, size, "%03u", (unsigned int)msPart);
            if (writtenMs <= 0)
            {
                memset(buffer, 0, bufSize);
                break;
            }
        }

        *written = writtenMain + writtenMs;
        result = true;
    } while (0);

    return result;
}

void fmt_push_single_node(FmtUnitNode* node, LogOutputIdEnum output)
{
    if (node == NULL)
    {
        return;
    }

    if (outFormats[output] == NULL)
    {
        outFormats[output] = node;
    }
    else
    {
        FmtUnitNode* curr = outFormats[output];
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = node;
    }
}

bool fmt_push_nodes(void* arg)
{
    bool result = false;

    // readability
    FmtParser* parser = (FmtParser*)arg;
    int output = parser->output;
    FmtUnitsEnum unit = parser->unit;
    long align = parser->align;
    const char* color = parser->color;
    char* extOption = parser->extOption;
    char* gap = parser->accumulateBuff;
    size_t gapLen = strlen(gap);

    FmtUnitNode* gapNode = NULL;
    FmtUnitNode* unitNode = NULL;

    do
    {
        if ((gapLen == 0) && (unit == FMT_UNIT_MAX_E))
        {
            // nothing to push: interpret as success
            result = true;
            break;
        }
        
        // create gap node
        if (gapLen)
        {
            gapNode = (FmtUnitNode*)calloc(1, sizeof(FmtUnitNode));
            if (gapNode == NULL)
            {   
                break;
            }

            gapNode->unit = FMT_UNIT_MAX_E;  // indicator that node is gap and not unit
            memcpy(gapNode->gap, gap, gapLen);
        }
        
        if (unit != FMT_UNIT_MAX_E)
        {
            unitNode = (FmtUnitNode*)calloc(1, sizeof(FmtUnitNode));;
            if (unitNode == NULL)
            {   
                break;
            }

            unitNode->unit = unit;
            unitNode->alignment = align;
            unitNode->color = color;
            memcpy(unitNode->extOption, extOption, strlen(extOption));
        }
        
        // push nodes:
        //   1. order 'gap' -> 'node' is important 
        //   2. NULL node might be passed safety
        fmt_push_single_node(gapNode, output);
        fmt_push_single_node(unitNode, output);

        result = true;
    } while (0);

    if (!result)
    {
        if (gapNode != NULL)
        {
            free(gapNode);
        }

        if (unitNode != NULL)
        {
            free(unitNode);
        }
    }

    return result;
}

FmtUnitsEnum fmt_find_unit(const char* string, size_t length)
{
    FmtUnitsEnum unit;
    
    for (unit = (FmtUnitsEnum)0; unit < FMT_UNIT_MAX_E; ++unit)
    {
        if (strncmp(string, fmtUnitMnemonics[unit], length) == 0)
        {
            break;
        }
    }

    return unit;
}

bool fmt_verify_timestamp_option(const char* format)
{   
    bool result = false;
    
    if (strlen(format) != 0)
    {
        // format string must be modifiable
        char inBuffer[FMT_BUFF_SIZE] = { 0 };
        memcpy(inBuffer, format, strlen(format));
        
        char outBuffer[FMT_BUFF_SIZE] = { 0 };
        size_t written = 0;
        result = get_timestamp(inBuffer, outBuffer, sizeof(outBuffer), &written);
    }

    // debug
    // printf("Timestamp = '%s'\n", buffer);

    return result;
}

bool fmt_verify_extended_option(FmtUnitsEnum unit, bool isExtended, const char* option)
{
    bool result = false;

    switch (unit)
    {
        case FMT_TIMESTAMP_E:
        {
            // see strftime() format
            result = isExtended && fmt_verify_timestamp_option(option);
        }
        break;

        default:
        {
            // unit which haven't extended options
            if (!isExtended)
            {
                result = true;
            }
        }
        break;
    }    
    
    return result;
}

bool fmt_parse_unit(FmtParser* parser, bool lastExists)
{
    bool result = false;

    // unit pattern = %<alignment>:<color>:<unit>[{extendedOption}]
    FmtUnitsEnum unit = FMT_UNIT_MAX_E;
    long align = 0;
    const char* color = NULL;

    // deal only with unit without special symbols
    const char* inputString = parser->currentBuff + FMT_UNIT_START_LEN;      
    size_t inputLen = parser->currentIndex - FMT_UNIT_CTRL_LEN + 1;

    do
    {      
        // find separators
        size_t separIndecies[FMT_UNIT_MAX_SEPARS] = { 0 };
        int actSeparCount = 0;
        size_t idx;
        for (idx = 0; idx < inputLen; ++idx)
        {
            if (inputString[idx] == FMT_UNIT_SEPAR)
            {
                ++actSeparCount;
                if (actSeparCount > FMT_UNIT_MAX_SEPARS)
                {
                    break;
                }
                separIndecies[actSeparCount - 1] = idx;
            }
        }
        
        // unit is necessary part: check unit and extended option
        if (actSeparCount >= 0)
        {
            const char* unitString = actSeparCount ? (inputString + (separIndecies[actSeparCount - 1] + 1)) : inputString;
            size_t unitLen = inputLen - (unitString - inputString) - (lastExists ? 1 : 0);
            size_t extLen;

            // detect extended option
            char lastChar = unitString[unitLen - 1];
            char* extOption;
            if (lastChar == FMT_EXT_OPT_LAST)
            {
                extOption = strstr(unitString, FMT_EXT_OPT_FIRST_STR);
                if (!extOption)
                {
                    break;
                }

                extLen = unitLen - (extOption - unitString);
                unitLen = extOption - unitString;
                memcpy(parser->extOption, (extOption + 1), extLen - 2);
            }
            
            // parse unit
            unit = fmt_find_unit(unitString, unitLen);
            if (unit == FMT_UNIT_MAX_E)
            {
                printf("ERROR = unknown unit: '%s' len=%zu\n", unitString, unitLen);
                break;
            }

            // verify extended option
            bool isExtended = (lastChar == FMT_EXT_OPT_LAST);
            isExtended = fmt_verify_extended_option(unit, isExtended, parser->extOption);
            if (!isExtended)
            {
                printf("ERROR = extended option is invalid: '%s'\n", parser->extOption);
            }    
        }

        // one separator = alignment is passed
        if (actSeparCount >= 1)
        {
            if (separIndecies[0] == 0)
            {
                // only separator is given = default alignment is used
                align = 0;           
            }
            else
            {
                char* longEnd;
                align = strtol(inputString, &longEnd, 10);
                
                if (errno == ERANGE)
                {
                    printf("ERROR = alignment overflow \n");
                    break;
                }

                if (*longEnd != FMT_UNIT_SEPAR)
                {
                    printf("ERROR = invalid alignment value\n");
                    break;
                }
            }
        }

        // one separator = alignment is passed
        if (actSeparCount >= 2)
        {
            if (separIndecies[actSeparCount - 1] == (separIndecies[actSeparCount - 2] + 1))
            {
                // epmty color is given = default is used
                color = NULL;
            }
            else
            {
                const char* colorString = inputString + (separIndecies[actSeparCount - 2] + 1);
                size_t colorLen = separIndecies[actSeparCount - 1] - (separIndecies[actSeparCount - 2] + 1);

                char* colorBuff = (char*)calloc(1, colorLen + 1);
                if (!colorBuff)
                {
                    break;
                }
                memcpy(colorBuff, colorString, colorLen);
                color = colorBuff;
            }
        }

        result = true;
    } while (0);

    parser->unit = unit;
    parser->align = align;
    parser->color = color;

    // debug
    // printf("unit=%s align=%li color=%scolor\033[m ext=%s\n", fmtUnitMnemonics[unit], align, color, parser->extOption);

    return result;
}

bool fmt_handle_unit(FmtParser* parser)
{
    bool result = false;
    
    do
    {
        // empty unit
        FMT_ERROR_IF_TRUE(parser->currentIndex == FMT_UNIT_CTRL_LEN);

        bool lastExist = (parser->currentBuff[parser->currentIndex - 1] == FMT_UNIT_LAST);
        result = fmt_parse_unit(parser, lastExist);
        FMT_ERROR_IF_FALSE(result);

        // debug
        // printf("GAP   '%s'\n", parser->accumulateBuff);
        // printf("UNIT  '%d:%s:%s'\n",  
        //    parser->align, 
        //    fmtUnitMnemonics[parser->unit],
        //    parser->extOption);
        
        // main work
        parser->callback(parser);
        FMT_ERROR_IF_FALSE(result);

        // prepare so that parsing might be continued
        fmt_parser_clear_buffers(parser);
        fmt_parser_change_state(parser, FMT_PARSE_GAP);

        /*
            Comment.    If consider whitespace as unit end and as
            a meaningful symbol for following gap, the problem occurs:
            no gap might be placed next to unit without whitespace and
            therefore more complicated parcing logic is required. So
            if smbd wants 1 space after unit should pass 2 instead.

            if (lastExist)
            {
                // last unit symbol is also meaningful for following gap
                parser->currentBuff[0] = FMT_UNIT_LAST;
                ++parser->currentIndex;
            }
        */ 

        result = true;
    } while (0);
    
    return result;
}

void fmt_parser_change_state(FmtParser* parser, FmtParserStateEnum newState)
{
    parser->currentState = newState;
}

void fmt_parser_stop_with_error(FmtParser* parser)
{
    fmt_parser_change_state(parser, FMT_END);
    parser->parsed = false;
}

bool fmt_parser_flush_current(FmtParser* parser)
{
    bool result = false;
    size_t length = strlen(parser->currentBuff);

    // check if accumulate buffer available space is enough
    // reserve last index for null-terminator
    size_t accumulateAvailable = FMT_BUFF_SIZE - parser->accumulateIndex - 1;  
    if (accumulateAvailable >= length)
    {
        memcpy(parser->accumulateBuff + parser->accumulateIndex, parser->currentBuff, length);
        memset(parser->currentBuff, 0, length);

        parser->accumulateIndex += length;
        parser->currentIndex = 0;
        result = true;
    }

    return result;
}

bool fmt_parser_write_char(FmtParser* parser)
{
    bool result = false;
    
    if (parser->currentIndex < (FMT_BUFF_SIZE - 1))  // reserve last index for null-terminator
    {
        parser->currentBuff[parser->currentIndex] = parser->string[parser->index];
        ++parser->currentIndex;
        result = true;
    }
    
    return result;
}

void fmt_parser_clear_buffers(FmtParser* parser)
{
    memset(parser->currentBuff, 0, strlen(parser->currentBuff));
    memset(parser->accumulateBuff, 0, strlen(parser->accumulateBuff));
    memset(parser->extOption, 0, strlen(parser->extOption));
    parser->currentIndex = 0;
    parser->accumulateIndex = 0;
    parser->unit = FMT_UNIT_MAX_E;
}

void fmt_parser_handle_signal(FmtParser* parser, FmtParserSignalEnum signal)
{
    switch (signal)
    {
        case FMT_SIGNAL_NONE:
        {
            // nothing
        }
        break;
        
        case FMT_SIGNAL_UNIT_FIRST:
        {
            bool result;
            
            switch (parser->currentState)
            {
                case FMT_PARSE_GAP:
                {
                    result = fmt_parser_flush_current(parser);
                    FMT_ERROR_IF_FALSE(result);
                    fmt_parser_change_state(parser, FMT_PARSE_UNIT);
                }
                break;

                case FMT_PARSE_EXT_OPT:
                {
                    // do nothing because we are in a context which
                    // FMT_SIGNAL_UNIT_FIRST haven't special effect in
                }
                break;

                // special case: two units in a row, 
                // so FMT_UNIT_LAST symbol doesn't present
                case FMT_PARSE_UNIT:
                {
                    result = fmt_handle_unit(parser);
                    FMT_ERROR_IF_FALSE(result);
                    fmt_parser_change_state(parser, FMT_PARSE_UNIT);
                }
                break;

                default:
                {
                    // invalid state
                }
                break;

            }
                    
            result = fmt_parser_write_char(parser);
            FMT_ERROR_IF_FALSE(result);
        }
        break;
        
        case FMT_SIGNAL_EXT_OPT_FIRST:
        {           
            bool result = fmt_parser_write_char(parser);
            FMT_ERROR_IF_FALSE(result);

            if (parser->currentState == FMT_PARSE_UNIT)
            {
                fmt_parser_change_state(parser, FMT_PARSE_EXT_OPT);
            }
        }
        break;

        case FMT_SIGNAL_EXT_OPT_LAST:
        {
            bool result = fmt_parser_write_char(parser);
            FMT_ERROR_IF_FALSE(result);

            if (parser->currentState == FMT_PARSE_EXT_OPT)
            {
                fmt_parser_change_state(parser, FMT_PARSE_UNIT);
            }
        }
        break;

        case FMT_SIGNAL_UNIT_LAST:
        {  
            bool result = fmt_parser_write_char(parser);
            FMT_ERROR_IF_FALSE(result);

            switch (parser->currentState)
            {
                case FMT_PARSE_UNIT:
                case FMT_PARSE_EXT_OPT:

                {               
                    result = fmt_handle_unit(parser);
                    FMT_ERROR_IF_FALSE(result);
                    break;
                }

                default:
                {
                    // invalid state
                    break;
                }
            }
        }
        break;

        case FMT_SIGNAL_END_STRING:
        {
            bool result;
            
            switch (parser->currentState)
            {
                case FMT_PARSE_UNIT:
                case FMT_PARSE_EXT_OPT:
                {
                    if (parser->currentIndex != 1)
                    {
                        // special case: unit ends simultaneously with the format
                        // string itself, so FMT_UNIT_LAST symbol doesn't present
                        result = fmt_handle_unit(parser);
                        break;
                    }
                }

                default:
                {
                    result = fmt_parser_flush_current(parser);
                    FMT_ERROR_IF_FALSE(result);

                    // debug
                    // printf("GAP   '%s'\n", parser->accumulateBuff);

                    result = parser->callback(parser);
                    FMT_ERROR_IF_FALSE(result);
                }
            }
            
            // end of parsing
            parser->parsed = result;
            fmt_parser_clear_buffers(parser);
            fmt_parser_change_state(parser, FMT_END);
        }
        break;
        
        case FMT_SIGNAL_REGULAR:
        {          
            bool result = fmt_parser_write_char(parser);
            FMT_ERROR_IF_FALSE(result);
        }
        break;

        default:
        {
            // nothing
        }
        break;
    }
}

void fmt_parse_format(FmtParser* parser)
{    
    while (parser->currentState != FMT_END)
    {          
        switch (parser->string[parser->index])
        {
            case FMT_UNIT_FIRST:
                fmt_parser_handle_signal(parser, FMT_SIGNAL_UNIT_FIRST);
                break;

            case FMT_EXT_OPT_FIRST:
                fmt_parser_handle_signal(parser, FMT_SIGNAL_EXT_OPT_FIRST);
                break;

            case FMT_EXT_OPT_LAST:
                fmt_parser_handle_signal(parser, FMT_SIGNAL_EXT_OPT_LAST);
                break;

            case FMT_UNIT_LAST:
                fmt_parser_handle_signal(parser, FMT_SIGNAL_UNIT_LAST);
                break;

            case FMT_END_STRING:
                fmt_parser_handle_signal(parser, FMT_SIGNAL_END_STRING);
                break;

            default:    // regular
                fmt_parser_handle_signal(parser, FMT_SIGNAL_REGULAR);
                break;
        }

        ++parser->index;
    }
}



int main()
{
    LogOutputIdEnum output = LOG_OUTPUT_ID_STDOUT_E;
    const char* fmt = "%-15:\033[38;5;26m:timestamp{%T.%f}  %filename  %line   %-12:\033[38;5;166m:thread   [ %severity  ]  >>  %message%endl";
    bool result = logging_set_format(output, fmt);
    
    // debug
    // printf("result=%s\n", result ? "true" : "false");

    if (result)
    {
        ERR("%s", "something went wrong");
    }

    return 0;
}