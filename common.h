// printf newline
#include <stdarg.h>

inline void println( const char* format, ... )
{
    va_list args;
    va_start( args, format );
    vprintf( format, args );
    printf("\n");
    va_end( args );
}
