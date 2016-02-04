/*
 * Interpolator.h
 *
 *  Created on: Jan 22, 2013
 *      Author: chenchu
 */

#ifndef INTERPOLATOR_H_
#define INTERPOLATOR_H_
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HPP_INTERPOL
#define HPP_INTERPOL

template<class T> class Interpolator
{

public:
    /*
        Linear interpolation
        target  - the target point, 0.0 - 1.0
        v       - a pointer to an array of size 2 containg the two values

    */

    inline static T Linear(float target, T *v)
    {
        return (T)(target*(v[1])+ (T(1.0f) - target)*(v[0]));
    }
    /*
        BiLinear interpolation, linear interpolation in 2D
        target  - a 2D point (X,Y)
        v       - an array of size 4 containg values cockwise around the square starting from bottom left

        cost: performs 3 linear interpolations
    */
    inline static T Bilinear(float *target, T *v)
    {
        T v_prime[2] = {
                        Linear(target[1], &(v[0])),
                        Linear(target[1], &(v[2]))
                       };

        return Linear(target[0], v_prime);

    }

    /*
        TriLinear interpolation, linear interpolation in 2D
        target  - a 3D point (X,Y)
        v       - an array of size 8 containg the values of the 8 corners
                    of a cube defined as two faces: 0-3 face one (front face)
                                                    4-7 face two (back face)

        cost: 7 linear interpolations
    */

    inline static T Trilinear(float *target, T *v)
    {
        T v_prime[2] = {
                         Bilinear(&(target[0]), &(v[0])),
                         Bilinear(&(target[1]), &(v[4]))
                       };

        return Linear(target[2], v_prime);
    }
};

#endif // HPP_INTERPOL

#endif /* INTERPOLATOR_H_ */
