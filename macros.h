#ifndef MACROS_H
#define MACROS_H

// printf newline
#include <vector>
#include <string>
#include <iostream>
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

template<typename T>
void print_vec( const std::vector<T> &v )
{
	for (size_t i=0; i<v.size(); i++)
		std::cout << v[i] << " ";
	std::cout << std::endl;
}

template<class T>
void print_array( T *v, int n )
{
	for (size_t i=0; i<n; i++)
		std::cout << v[i] << " ";
	std::cout << std::endl;
}




}; // namespace jclib

#endif // MACROS_H
