#include <stdio.h>
#include <string.h>
#include <stdbool.h>


typedef enum FmtUnits
{
    // location
    FMT_FILE,
    FMT_FUNCTION,
    FMT_LINE,
    FMT_THREAD,
    // logger
    FMT_LOGGER,
    FMT_SEVERITY,
    // timestamp
    FMT_YEAR,
    FMT_MONTH,
    FMT_DAY,
    FMT_HOUR,
    FMT_MINUTE,
    FMT_SECOND,
    FMT_MSEC,
    // content
    FMT_MESSAGE,

    FMT_UNIT_MAX
} FmtUnits;

const char* fmt_unit_mnemonics[FMT_UNIT_MAX] =
{
    [FMT_FILE]      = "file"    ,
    [FMT_FUNCTION]  = "func"    ,
    [FMT_LINE]      = "line"    ,
    [FMT_THREAD]    = "thread"  ,
    [FMT_LOGGER]    = "logger"  ,
    [FMT_SEVERITY]  = "sever"   ,
    [FMT_YEAR]      = "year"    ,
    [FMT_MONTH]     = "month"   ,
    [FMT_DAY]       = "day"     ,
    [FMT_HOUR]      = "hour"    ,
    [FMT_MINUTE]    = "minute"  ,
    [FMT_SECOND]    = "second"  ,
    [FMT_MSEC]      = "msec"    ,
    [FMT_MESSAGE]   = "message" ,
};

#ifdef DEBUG
    #define _S(string) printf("%s\n", string)
    #define _C(symbol) printf("char=%c", symbol)
#else 
    #define _S(string)
    #define _C(symbol)
#endif


#define FMT_BUFFERS_SIZE     (64)   // synthetic limit, no sence to do more
#define FMT_UNIT_FIRST       '$' 
#define FMT_UNIT_SECOND      '{' 
#define FMT_UNIT_LAST        '}'
#define FMT_UNIT_START_LEN   (2)    // FMT_UNIT_FIRST, FMT_UNIT_SECOND 
#define FMT_UNIT_END_LEN     (1)    // FMT_UNIT_LAST
#define FMT_UNIT_CTRL_LEN    (FMT_UNIT_START_LEN + FMT_UNIT_END_LEN) 
#define FMT_END_STRING      '\0'

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

typedef enum FmtParserState
{    
    FMT_PARSE_GAP,
    FMT_FOUND_UNIT_FIRST,
    FMT_PARSE_UNIT,
    FMT_END
} FmtParserState;

typedef enum FmtParserSignal
{
    FMT_SIGNAL_NONE,
    FMT_SIGNAL_UNIT_FIRST,
    FMT_SIGNAL_UNIT_SECOND,
    FMT_SIGNAL_UNIT_LAST,
    FMT_SIGNAL_END_STRING,
    FMT_SIGNAL_REGULAR
} FmtParserSignal;

typedef void (*callback)();
void xxx()
{
    printf("  XXX \n");
}


typedef struct FmtParser
{
    bool parsed;
    callback f;

    FmtParserState currentState;
    const char* string;
    size_t index;

    char currentBuff[FMT_BUFFERS_SIZE];        
    char accumulateBuff[FMT_BUFFERS_SIZE];
    size_t currentIndex;      
    size_t accumulateIndex;

} FmtParser;



FmtUnits fmt_find_unit(const char* string, size_t length)
{
    FmtUnits unit;
    
    for (unit = (FmtUnits)0; unit < FMT_UNIT_MAX; ++unit)
    {
        if (strncmp(string, fmt_unit_mnemonics[unit], length) == 0)
        {
            break;
        }
    }

    return unit;
}

bool fmt_parser_write_char(FmtParser* parser)
{
    bool result = false;
    
    if (parser->currentIndex < (FMT_BUFFERS_SIZE - 1))  // reserve last index for null-terminator
    {
        parser->currentBuff[parser->currentIndex] = parser->string[parser->index];
        ++parser->currentIndex;
        result = true;
    }
    
    return result;
}

bool fmt_parser_flush_current(FmtParser* parser)
{
    bool result = false;
    size_t length = strlen(parser->currentBuff);

    // check if accumulate buffer available space is enough
    // reserve last index for null-terminator
    size_t accumulateAvailable = FMT_BUFFERS_SIZE - parser->accumulateIndex - 1;  
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

void fmt_parser_clear_buffers(FmtParser* parser)
{
    memset(parser->currentBuff, 0, strlen(parser->currentBuff));
    memset(parser->accumulateBuff, 0, strlen(parser->accumulateBuff));
    parser->currentIndex = 0;
    parser->accumulateIndex = 0;
}

void fmt_parser_change_state(FmtParser* parser, FmtParserState newState)
{
    parser->currentState = newState;
}

void fmt_parser_stop_with_error(FmtParser* parser)
{
    fmt_parser_change_state(parser, FMT_END);
    parser->parsed = false;
}

void fmt_parser_handle_signal(FmtParser* parser, FmtParserSignal signal)
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
            FMT_ERROR_IF_FALSE(fmt_parser_flush_current(parser));
            FMT_ERROR_IF_FALSE(fmt_parser_write_char(parser));
            fmt_parser_change_state(parser, FMT_FOUND_UNIT_FIRST);
        }
        break;

        case FMT_SIGNAL_UNIT_SECOND:
        {
            fmt_parser_write_char(parser);
            if (parser->currentState == FMT_FOUND_UNIT_FIRST)
                fmt_parser_change_state(parser, FMT_PARSE_UNIT);
        }
        break;
        
        case FMT_SIGNAL_UNIT_LAST:
        {
            FMT_ERROR_IF_FALSE(fmt_parser_write_char(parser));

            if (parser->currentState == FMT_PARSE_UNIT)
            {
                // empty unit
                FMT_ERROR_IF_TRUE(parser->currentIndex == FMT_UNIT_CTRL_LEN);

                FmtUnits unit = fmt_find_unit(parser->currentBuff + FMT_UNIT_START_LEN, parser->currentIndex - FMT_UNIT_CTRL_LEN);
                FMT_ERROR_IF_TRUE(unit == FMT_UNIT_MAX)    // invalid unit
   
                // debug
                printf("%s \n", parser->accumulateBuff);            // replace: handle previous GAP (in accumulateBuffer)
                printf("%s = %d\n", parser->currentBuff, unit);     // replace: handle current UNIT (in currentBuffer)
                parser->f();

                fmt_parser_clear_buffers(parser);
                fmt_parser_change_state(parser, FMT_PARSE_GAP);
            }
        }
        break;
        
        case FMT_SIGNAL_END_STRING:
        {
            FMT_ERROR_IF_FALSE(fmt_parser_flush_current(parser));

            // debug
            printf("%s \n", parser->accumulateBuff);        // replace: handle previous GAP (in accumulateBuffer)
            parser->f();

            parser->parsed = true;
            fmt_parser_clear_buffers(parser);
            fmt_parser_change_state(parser, FMT_END);
        }
        break;
        
        case FMT_SIGNAL_REGULAR:
        {
            FMT_ERROR_IF_FALSE(fmt_parser_write_char(parser));
        }
        break;

        default:
        {
            // nothing
        }
        break;
    }

    _S(parser->currentBuff);
}

FmtParser parse_format(const char* format)
{
    FmtParser parser = 
    { 
        .parsed             = false, 
        .f                  = xxx,
        .string             = format,
        .index              = 0,
        .currentBuff        = { 0 },
        .accumulateBuff     = { 0 },
        .currentIndex       = 0,
        .accumulateIndex    = 0,
        .currentState       = FMT_PARSE_GAP
    };
    
    if ((format == NULL) || (strlen(format) == 0))
    {
        return parser;
    }
    
    while (parser.currentState != FMT_END)
    {
        _C(parser.string[parser.index]);
        
        switch (parser.string[parser.index])
        {
            case FMT_UNIT_FIRST:
                _S("  > FMT_UNIT_FIRST");
                fmt_parser_handle_signal(&parser, FMT_SIGNAL_UNIT_FIRST);
                break;

            case FMT_UNIT_SECOND:
                _S("  > FMT_UNIT_SECOND");
                fmt_parser_handle_signal(&parser, FMT_SIGNAL_UNIT_SECOND);
                break;

            case FMT_UNIT_LAST:
                _S("  > FMT_UNIT_LAST");
                fmt_parser_handle_signal(&parser, FMT_SIGNAL_UNIT_LAST);
                break;

            case FMT_END_STRING:
                _S("  > FMT_END_STRING");
                fmt_parser_handle_signal(&parser, FMT_SIGNAL_END_STRING);
                break;

            default:    // regular
                _S("  > REGULAR");
                fmt_parser_handle_signal(&parser, FMT_SIGNAL_REGULAR);
                break;
        }

        ++parser.index;
    }

    return parser;
}

int main()
{
    const char* fmt = "gap$gap${un${func}gap${gap${line}${...";
    FmtParser parser = parse_format(fmt);
    printf("result=%s\n", parser.parsed ? "true" : "false");

    return 0;
}
