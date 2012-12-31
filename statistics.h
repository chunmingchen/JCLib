#ifndef STATISTICS_H
#define STATISTICS_H


#include <math.h>
#include <numeric>

#include <assert.h>

namespace JCLib
{
	template <class InputIterator>
	double getSum( InputIterator first, InputIterator last ) {
		double sum_x=0;
		for ( InputIterator i=first; i!=last; ++i)
		{
			sum_x += *i;
		}
		return sum_x;
	}

	template <class InputIterator>
	double getMean ( InputIterator first, InputIterator last ) {
		double sum_x=0;
		int dist=0;
		for ( InputIterator i=first; i!=last; ++i)
		{
			sum_x += *i;
			dist++;
		}
		return sum_x / dist;
	}

	template <class InputIterator>
	double getDeviation ( InputIterator first, InputIterator last ) {
		double sum_x=0, sum_square=0;
		int dist=0;
		for ( InputIterator i=first; i!=last; ++i)
		{
			sum_x += *i;
			sum_square += (*i)*(*i);
			dist++;
		}
		assert(dist>0);
		return sqrt( sum_square/dist -  sum_x*sum_x/(dist*dist) );
	}
};

#endif
