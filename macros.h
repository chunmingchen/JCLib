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

inline std::string strprintf( const char * format, ... )
{
    va_list args;
    char *s = new char[strlen(format)+1024];
    va_start( args, format );
    vsprintf( s, format, args );
    std::string str = s;
    delete[] s;
    va_end( args );
    return str;
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

// source: http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
template <class InputIterator>
std::vector<size_t> sort_indexes(InputIterator begin, InputIterator end) {

  // initialize original index locations
  std::vector<size_t> idx(end-begin);
  for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&begin](size_t i1, size_t i2) {return *(begin+i1) < *(begin+i2);});

  return idx;
}

}; // namespace jclib

#endif // MACROS_H
