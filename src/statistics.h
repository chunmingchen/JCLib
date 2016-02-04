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

	// divided by n
	template <class InputIterator>
	double getDeviation1 ( InputIterator first, InputIterator last ) {
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

	// divided by n-1
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
		return sqrt( sum_square/(dist-1) -  sum_x*sum_x/(dist*(dist-1)) );
	}

	/* boxmuller.c           Implements the Polar form of the Box-Muller
	                         Transformation

	                      (c) Copyright 1994, Everett F. Carter Jr.
	                          Permission is granted by the author to use
				  this software for any application provided this
				  copyright notice is preserved.

	*/
	template <class T>
	T box_muller(T m, T s)	/* normal random variate generator */
	{				        /* mean m, standard deviation s */
		T x1, x2, w, y1;
		static T y2;
		static char use_last = 0;

		if (use_last)		        /* use value from previous call */
		{
			y1 = y2;
			use_last = 0;
		}
		else
		{
			do {
				x1 = 2.0 * (T)rand()/RAND_MAX - 1.0;
				x2 = 2.0 * (T)rand()/RAND_MAX - 1.0;
				w = x1 * x1 + x2 * x2;
			} while ( w >= 1.0 );

			w = sqrt( (-2.0 * log( w ) ) / w );
			y1 = x1 * w;
			y2 = x2 * w;
			use_last = 1;
		}

		return( m + y1 * s );
	}



};

#endif
