#ifndef MACROS_H
#define MACROS_H

// printf newline
#include <stdarg.h>

namespace JCLib{

inline void println( const char* format, ... )
{
    va_list args;
    va_start( args, format );
    vprintf( format, args );
    printf("\n");
    va_end( args );
}


}; // namespace jclib

#endif // MACROS_H
