/*
 *  gztypes.h
 *  RayTracing
 *
 *  Created by Chun-Ming Chen on 10/11/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#ifndef __GZ_TYPES__
#define __GZ_TYPES__

namespace JCLib {

typedef float REAL;
#define EPS 1e-5

#define CLAMP(a,b,c) ((a)<(b)?(b): (a)>(c)?(c): (a))


struct Pixel {
	union{
		unsigned char v[3]; //val
		struct{
			unsigned char b,g,r;
		};
	};
	Pixel(unsigned char _r, unsigned char _g,unsigned char _b) {r=_r;g=_g;b=_b;}
	Pixel() {}
};
    
};

#endif
