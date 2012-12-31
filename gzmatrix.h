/*
 *  gzmatrix.h
 *  RayTracing
 *
 *  Created by Chun-Ming Chen on 10/4/09.
 *  Copyright 2009. All rights reserved.
 *
 */
#ifndef __GZ_MATRIX__
#define __GZ_MATRIX__
#include "gztypes.h"
#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

namespace JCLib {

class GzVector;
class GzCoord {
public:
    union{
        REAL p[4];
        struct{
            REAL x,y,z,w;
        };
    };
    
    GzCoord() {x=y=z=0; w=1;}
    GzCoord(REAL _x,REAL _y, REAL _z, REAL _h) {x=_x;y=_y;z=_z;w=_h;}
    GzCoord(REAL _x,REAL _y, REAL _z) {x=_x,y=_y,z=_z,w=1.0;}
	
	void divideW() {x/=w; y/=w; z/=w; w=1.0;}
               
    inline GzCoord operator*(REAL b) const{
        return GzCoord(x*b, y*b, z*b, w*b);
    }
	inline REAL dot(const GzCoord &p) const {
		return (x*p.x + y*p.y + z*p.z + w*p.w);
	}

    inline void print() const{
        printf("coord=(%f %f %f %f)\n", x,y,z,w);
    }
};

class GzVector: public GzCoord{
public:
    GzVector() {x=y=z=w=0;}
    GzVector(REAL _x,REAL _y, REAL _z) {x=_x,y=_y,z=_z,w=0.0;}
    GzVector(const GzCoord &c) {memcpy(p, c.p, sizeof(REAL)*3);w=0;}
    
    // normalize coordinate (x,y,z), ignore the 4th axis
    inline GzVector normalize() const {double d=getLength(); return GzVector(x/d,y/d,z/d); }
    inline GzVector &normalizeMe() {double d=sqrt(x*x+y*y+z*z); x/=d; y/=d; z/=d; return *this;}
    inline double getLength() const {return sqrt(x*(double)x+y*(double)y+z*(double)z); }
    
	// ignore w!
    inline GzVector operator*(REAL b) const{
        return GzVector(x*b, y*b, z*b);
    }
    
    inline GzVector operator/(REAL b) const {
        return GzVector(x/b, y/b, z/b);
    }
    
    inline GzVector cross(const GzVector &b) const{
        return GzVector(y*b.z-z*b.y, z*b.x-x*b.z, x*b.y-y*b.x);
    }
    
    inline REAL dot(const GzVector &b) const{
        return x*b.x+y*b.y+z*b.z;
    }
    
    inline GzVector operator+(const GzVector &b) const{
        return GzVector(x+b.x, y+b.y, z+b.z);
    }
    inline GzVector operator-(const GzVector &b) const{
        return GzVector(x-b.x, y-b.y, z-b.z);
    }
    inline GzVector operator+=(const GzVector &b) {
        x+=b.x; y+=b.y; z+=b.z;
        return *this;
    }
    // 2*normal*(normal dot this) - this
    inline GzVector getReflection_norm(const GzVector &normal_norm) const {
        return (normal_norm*(this->dot(normal_norm)*2.0) - (*this)).normalize();
    }

};

inline  GzCoord operator+(const GzCoord &a, const GzVector &b) {
    return GzCoord(a.x+b.x, a.y+b.y, a.z+b.z);
    
}
inline  GzVector operator-(const GzCoord &a, const GzCoord &b)  {
    return GzVector(a.x-b.x,a.y-b.y,a.z-b.z);
}

const REAL MAT_IDENTITY[16] = 
    {1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1};

    
#if 1
class GzMatrix {
    
public:
    REAL mat[4][4];
    inline GzMatrix &genIdentity()
    {
        memcpy(mat, MAT_IDENTITY, sizeof(REAL)*16);
        return *this;
    }
    inline void genZero()
    {
        memset(mat, 0, sizeof(REAL)*16);
    }
	inline void setRow(int r, GzCoord p) {
		memcpy(mat[r], p.p, sizeof(REAL)*4);
	}
	inline REAL *getRow(int r) {
		return mat[r];
	}
    
    // Create rotate matrix : rotate along x axis (CCW)
    // Pass back the matrix using mat value
    void genRotX(float degree)
    {
        float theta = degree*(float)M_PI/180.0f;
        float cosine = (float)cos(theta);
        float sine = (float)sin(theta);
        genIdentity();
        mat[1][1] = cosine;
        mat[1][2] = -sine;
        mat[2][1] = sine;
        mat[2][2] = cosine;
        
    }
    
    void genRotY(float degree)
    {
        // Create rotate matrix : rotate along y axis
        // Pass back the matrix using mat value
        float theta = (float)degree*(float)M_PI/180.0f;
        float cosine = (float)cos(theta);
        float sine = (float)sin(theta);
        genIdentity();
        mat[0][0] = cosine;
        mat[0][2] = sine;
        mat[2][0] = -sine;
        mat[2][2] = cosine;
        
    }
    
    
    void genRotZ(float degree)
    {
        // Create rotate matrix : rotate along z axis
        // Pass back the matrix using mat value
        float theta = degree*(float)M_PI/180;
        float cosine = (float)cos(theta);
        float sine = (float)sin(theta);
        genIdentity();
        mat[0][0] = cosine;
        mat[0][1] = -sine;
        mat[1][0] = sine;
        mat[1][1] = cosine;
    }
    
    
    void genTrx(const GzCoord &translate)
    {
        // Create translation matrix
        // Pass back the matrix using mat value
        
        genIdentity();
        mat[0][3] = translate.x;
        mat[1][3] = translate.y;
        mat[2][3] = translate.z;
        
    }
    
    
    void genScale(const GzCoord &scale)
    {
        // Create scaling matrix
        // Pass back the matrix using mat value
        genIdentity();
        mat[0][0] = scale.x;
        mat[1][1] = scale.y;
        mat[2][2] = scale.z;
    }
	inline void genScale(const REAL scale)
	{
        genIdentity();
        mat[0][0] = 
        mat[1][1] = 
        mat[2][2] = scale;
	}
    
    void genTranspose(const GzMatrix &m) 
    {
        int x,y;
        for (y=0; y<4; y++)
            for (x=0; x<4; x++)
                mat[y][x] = m.mat[x][y];
    }
    
    
    GzMatrix operator*(const GzMatrix &b) const
    {
        //out = mat x b
        GzMatrix out;
        int i,j,k;
        for (i=0; i<4; i++)
            for (j=0; j<4; j++){
                out.mat[i][j] = 0;
                for (k=0; k<4; k++)
                    out.mat[i][j] += mat[i][k]*b.mat[k][j];
            }
        return out;
    }
    
    
    GzMatrix &operator*=(const GzMatrix &b) 
    {
        // mat = mat x b
        return (*this) = (*this)*b;

    }
    
    GzCoord operator*(const GzCoord &b)  const
    {
        // c = mat x b
        GzCoord c;
        int i,j;
        for (i=0; i<4; i++)
        {
            c.p[i]=0;
            for (j=0; j<4; j++)
                c.p[i]+=mat[i][j]*b.p[j];
        }
        return c;
    }
    GzVector operator*(const GzVector &b) const { return (GzVector)operator*((GzCoord)b);}
    
	GzMatrix &operator*=(const REAL c) 
	{
		for (int i=0; i<16; i++) ((REAL *)mat)[i]*=c;
		return *this;
	}
    
	GzMatrix &operator-=(const GzMatrix m) 
	{
		for (int i=0; i<16; i++) ((REAL *)mat)[i]-=((REAL *)m.mat)[i];
		return *this;
	}
	GzMatrix &operator+=(const GzMatrix m) 
	{
		for (int i=0; i<16; i++) ((REAL *)mat)[i]+=((REAL *)m.mat)[i];
		return *this;
	}
    
    // gen tilt matrix (axis: ratating axis)
    // axis should be normalized!
    // reference: http://en.wikipedia.org/wiki/Rotation_matrix
    void genTilt(const GzVector &axis, REAL tilt) 
    {        
        const GzVector &u = axis;
        tilt = tilt*M_PI/180;
        double c = cos(tilt);
        double s = sin(tilt);
        genIdentity();
        mat[0][0] = u.x*u.x+(1-u.x*u.x)*c;
        mat[0][1] = u.x*u.y*(1-c)-u.z*s;
        mat[0][2] = u.x*u.z*(1-c)+u.y*s;
        mat[1][0] = u.x*u.y*(1-c)+u.z*s;
        mat[1][1] = u.y*u.y+(1-u.y*u.y)*c;
        mat[1][2] = u.y*u.z*(1-c)-u.x*s;
        mat[2][0] = u.x*u.z*(1-c)-u.y*s;
        mat[2][1] = u.y*u.z*(1-c)+u.x*s;
        mat[2][2] = u.z*u.z+(1-u.z*u.z)*c;
        
    }

	inline double det() const {
#define m mat
		const double m00=m[0][0], m01=m[0][1], m02=m[0][2], m03=m[0][3],
				m10=m[1][0], m11=m[1][1], m12=m[1][2], m13=m[1][3],
				m20=m[2][0], m21=m[2][1], m22=m[2][2], m23=m[2][3],
				m30=m[3][0], m31=m[3][1], m32=m[3][2], m33=m[3][3];
		return (m00*m11*m22*m33-m00*m11*m23*m32-m00*m21*m12*m33+m00*m21*m13*m32+m00*m31*m12*m23-m00*m31*m13*m22-
			m10*m01*m22*m33+m10*m01*m23*m32+m10*m21*m02*m33-m10*m21*m03*m32-m10*m31*m02*m23+m10*m31*m03*m22+
			m20*m01*m12*m33-m20*m01*m13*m32-m20*m11*m02*m33+m20*m11*m03*m32+m20*m31*m02*m13-m20*m31*m03*m12-
			m30*m01*m12*m23+m30*m01*m13*m22+m30*m11*m02*m23-m30*m11*m03*m22-m30*m21*m02*m13+m30*m21*m03*m12);
#undef m
	}
 
	//[  m11*m22*m33-m11*m23*m32-m21*m12*m33+m21*m13*m32+m31*m12*m23-m31*m13*m22, -m01*m22*m33+m01*m23*m32+m21*m02*m33-m21*m03*m32-m31*m02*m23+m31*m03*m22,  m01*m12*m33-m01*m13*m32-m11*m02*m33+m11*m03*m32+m31*m02*m13-m31*m03*m12, -m01*m12*m23+m01*m13*m22+m11*m02*m23-m11*m03*m22-m21*m02*m13+m21*m03*m12]
	//[ -m10*m22*m33+m10*m23*m32+m20*m12*m33-m20*m13*m32-m30*m12*m23+m30*m13*m22,  m00*m22*m33-m00*m23*m32-m20*m02*m33+m20*m03*m32+m30*m02*m23-m30*m03*m22, -m00*m12*m33+m00*m13*m32+m10*m02*m33-m10*m03*m32-m30*m02*m13+m30*m03*m12,  m00*m12*m23-m00*m13*m22-m10*m02*m23+m10*m03*m22+m20*m02*m13-m20*m03*m12]
	//[  m10*m21*m33-m10*m23*m31-m20*m11*m33+m20*m13*m31+m30*m11*m23-m30*m13*m21, -m00*m21*m33+m00*m23*m31+m20*m01*m33-m20*m03*m31-m30*m01*m23+m30*m03*m21,  m00*m11*m33-m00*m13*m31-m10*m01*m33+m10*m03*m31+m30*m01*m13-m30*m03*m11, -m00*m11*m23+m00*m13*m21+m10*m01*m23-m10*m03*m21-m20*m01*m13+m20*m03*m11]
	//[ -m10*m21*m32+m10*m22*m31+m20*m11*m32-m20*m12*m31-m30*m11*m22+m30*m12*m21,  m00*m21*m32-m00*m22*m31-m20*m01*m32+m20*m02*m31+m30*m01*m22-m30*m02*m21, -m00*m11*m32+m00*m12*m31+m10*m01*m32-m10*m02*m31-m30*m01*m12+m30*m02*m11,  m00*m11*m22-m00*m12*m21-m10*m01*m22+m10*m02*m21+m20*m01*m12-m20*m02*m11]
	// d = m00*m11*m22*m33-m00*m11*m23*m32-m00*m21*m12*m33+m00*m21*m13*m32+m00*m31*m12*m23-m00*m31*m13*m22-m10*m01*m22*m33+m10*m01*m23*m32+m10*m21*m02*m33-m10*m21*m03*m32-m10*m31*m02*m23+m10*m31*m03*m22+m20*m01*m12*m33-m20*m01*m13*m32-m20*m11*m02*m33+m20*m11*m03*m32+m20*m31*m02*m13-m20*m31*m03*m12-m30*m01*m12*m23+m30*m01*m13*m22+m30*m11*m02*m23-m30*m11*m03*m22-m30*m21*m02*m13+m30*m21*m03*m12

	void genInverse(const GzMatrix &matrix, bool noDivide=false) 
	{
#define m matrix.mat
		const double m00=m[0][0], m01=m[0][1], m02=m[0][2], m03=m[0][3],
				m10=m[1][0], m11=m[1][1], m12=m[1][2], m13=m[1][3],
				m20=m[2][0], m21=m[2][1], m22=m[2][2], m23=m[2][3],
				m30=m[3][0], m31=m[3][1], m32=m[3][2], m33=m[3][3];
#undef m
		double invdet = noDivide? 1.0 : 1.0 / matrix.det();
		mat[0][0] = (m11*m22*m33-m11*m23*m32-m21*m12*m33+m21*m13*m32+m31*m12*m23-m31*m13*m22)*invdet;
		mat[0][1] = -(m01*m22*m33-m01*m23*m32-m21*m02*m33+m21*m03*m32+m31*m02*m23-m31*m03*m22)*invdet;
		mat[0][2] = (m01*m12*m33-m01*m13*m32-m11*m02*m33+m11*m03*m32+m31*m02*m13-m31*m03*m12)*invdet;
		mat[0][3] = -(m01*m12*m23-m01*m13*m22-m11*m02*m23+m11*m03*m22+m21*m02*m13-m21*m03*m12)*invdet;
		mat[1][0] = -(m10*m22*m33-m10*m23*m32-m20*m12*m33+m20*m13*m32+m30*m12*m23-m30*m13*m22)*invdet;
		mat[1][1] = (m00*m22*m33-m00*m23*m32-m20*m02*m33+m20*m03*m32+m30*m02*m23-m30*m03*m22)*invdet;
		mat[1][2] = -(m00*m12*m33-m00*m13*m32-m10*m02*m33+m10*m03*m32+m30*m02*m13-m30*m03*m12)*invdet;
		mat[1][3] = (m00*m12*m23-m00*m13*m22-m10*m02*m23+m10*m03*m22+m20*m02*m13-m20*m03*m12)*invdet;
		mat[2][0] = (m10*m21*m33-m10*m23*m31-m20*m11*m33+m20*m13*m31+m30*m11*m23-m30*m13*m21)*invdet;
		mat[2][1] = -(m00*m21*m33-m00*m23*m31-m20*m01*m33+m20*m03*m31+m30*m01*m23-m30*m03*m21)*invdet;
		             
		mat[2][2] = (m00*m11*m33-m00*m13*m31-m10*m01*m33+m10*m03*m31+m30*m01*m13-m30*m03*m11)*invdet;
		mat[2][3] = -(m00*m11*m23-m00*m13*m21-m10*m01*m23+m10*m03*m21+m20*m01*m13-m20*m03*m11)*invdet;
		mat[3][0] = -(m10*m21*m32-m10*m22*m31-m20*m11*m32+m20*m12*m31+m30*m11*m22-m30*m12*m21)*invdet;
		mat[3][1] = (m00*m21*m32-m00*m22*m31-m20*m01*m32+m20*m02*m31+m30*m01*m22-m30*m02*m21)*invdet;
		mat[3][2] = -(m00*m11*m32-m00*m12*m31-m10*m01*m32+m10*m02*m31+m30*m01*m12-m30*m02*m11)*invdet;
		mat[3][3] = (m00*m11*m22-m00*m12*m21-m10*m01*m22+m10*m02*m21+m20*m01*m12-m20*m02*m11)*invdet;
 
	}
    
    void print() {
        int i;
        for (i=0; i<4; i++)
            printf("%f %f %f %f\n", mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
        printf("\n");
    }
};
#endif
    
    
};

#endif

