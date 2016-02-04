/*
 *  image.h
 *  RayTracing
 *
 *  Created by MiGi on 10/11/09.
 *  Copyright 2009 OSU. All rights reserved.
 *
 */
#ifndef __GZ_IMAGE__
#define __GZ_IMAGE__
#include <stdio.h>
#include "gztypes.h"

namespace JCLib {

class Image {
    Pixel *buffer;
public:
    int xdim,ydim;

    Image(int _xdim, int _ydim) 
    {
        xdim=_xdim, ydim=_ydim; 
        buffer = new Pixel[xdim*ydim];
    }
    ~Image() {delete[] buffer;}
    inline Pixel &getPixel(int x, int y) {return buffer[x%xdim+(y%ydim)*xdim]; }
    inline void setPixel(int x, int y, const Pixel &p) {buffer[x+y*xdim] = p;}

	Pixel &getPixel_repeat(int x, int y) {
		while (x>xdim) x-=xdim;
		while (y>ydim) y-=ydim;
		while (x<=0) x+=xdim;
		while (y<=0) y+=ydim;
		return getPixel(x,y);
	}

    inline int getXDim() {return xdim;}
    inline int getYDim() {return ydim;}
    
    void outputPPM(const char *path) {
        FILE* out = fopen(path, "w"); 
        if (!out)
        {
            fprintf(stderr, "Fail to open file:%s!\n", path);
            return;
        }
        fprintf(out, "P3\n"); 
        fprintf(out, "%d  %d\n", xdim, ydim); 
        fprintf(out, "%d\n", 255); 
        
        int x,y;
        for (y=0; y<ydim; y++)
            for (x=0; x<xdim; x++)
            {
                const Pixel &p = getPixel(x,y);
                fprintf(out, "%d %d %d\n", p.r, p.g, p.b); 
            }
        fclose(out); 
    }
    
    static Image *inputPPM(const char *path) {
        FILE *fin = fopen(path, "r");
        if (!fin)
            return NULL;
        char dum[256];
        int xdim, ydim;
		fgets(dum, 256, fin);
		fgets(dum, 256, fin);
        fscanf(fin, "%d %d %s", &xdim, &ydim, dum);
        Image *img = new Image(xdim, ydim);
        int x,y;
        for (y=0; y<ydim; y++)
            for (x=0; x<xdim; x++)
            {
                int r,g,b;
                Pixel &p = img->getPixel(x,y);
                fscanf(fin, "%d %d %d", &r,&g,&b);
                p.r = r; p.g=g; p.b=b;
            }
        fclose(fin);
        return img;
    }
    
    void bilinearGetPixel(Pixel &out, REAL &u, REAL &v)
    {
        /* bounds-test u,v to make sure nothing will overflow image array bounds */
        /* determine texture cell corner values and perform bilinear interpolation */
        /* set color to interpolated GzColor value and return */
        
        float pixU = u*(xdim-1),
        pixV = v*(ydim-1);
        int pixIU = (int)pixU,
        pixIV = (int)pixV;
        float s = pixU-pixIU,
		t = pixV-pixIV;
        Pixel
        &pA = getPixel(pixIU,pixIV),
        &pB = getPixel(pixIU+1,pixIV),
        &pC = getPixel(pixIU, pixIV+1),
        &pD = getPixel(pixIU+1, pixIV+1);
        
        out.r = (unsigned char)(s*t*pC.r + (1-s)*t*pD.r + s*(1-t)*pB.r + (1-s)*(1-t)*pA.r);
        out.g = (unsigned char)(s*t*pC.g + (1-s)*t*pD.g + s*(1-t)*pB.g + (1-s)*(1-t)*pA.g);
        out.b = (unsigned char)(s*t*pC.b + (1-s)*t*pD.b + s*(1-t)*pB.b + (1-s)*(1-t)*pA.b);
        
    }
};
    
};

#endif
