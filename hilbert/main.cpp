//
//  main.cpp
//  Hilbert
//
//  Created by Jimmy Chen on 12/2/11.
//  Copyright 2011 OSU. All rights reserved.
//

#include <assert.h>
#include <iostream>
#include "hilbert.h"



void show_idx_2d(int order)
{
    int w = 1<<order; // width
    int v[2];
    printf("2D: order=%d\n", order);
    for (v[1]=0; v[1]<w; v[1]++)
        for (v[0]=0; v[0]<w; v[0]++)
        {
            Point<2> p;
            p.v[0]=v[0];
            p.v[1] = v[1];
            printf("[%d,%d]==>%d\n", p.v[0], p.v[1], Hilbert2d::index(p, order));
        }
}
void show_idx_3d(int order)
{
    int w = 1<<order; // width
    int v[3];
    printf("3D: order=%d\n", order);
    for (v[2]=0; v[2]<w; v[2]++)
    for (v[1]=0; v[1]<w; v[1]++)
        for (v[0]=0; v[0]<w; v[0]++)
        {
            Point<3> p;
            p.v[0]=v[0];
            p.v[1] = v[1];
            p.v[2]=v[2];
            printf("[%d,%d,%d]==>%d\n", p.v[0], p.v[1], p.v[2], Hilbert3d::index(p, order));
        }
}

void show_pos_2d(int order)
{
    printf("2D: order=%d\n", order);
    int w = 1<<order; // width
    int n = w*w;
    for (int i=0; i<n; i++)
    {
        Point<2> p;
        p = Hilbert2d::pos(i, order);
        printf("%d==>[%d,%d]\n", i,p.v[0],p.v[1]);
    }
}

void show_pos_3d(int order)
{
    printf("3D: order=%d\n", order);
    int w = 1<<order; // width
    int n = w*w*w;
    for (int i=0; i<n; i++)
    {
        Point<3> p;
        p = Hilbert3d::pos(i, order);
        printf("%d==>[%d,%d,%d]\n", i,p.v[0],p.v[1],p.v[2]);
    }
}

void verify_2d(int order)
{
    int w = 1<<order; // width
    int n = w*w*w;
    for (int i=0; i<n; i++)
    {
        Point<2> p;
        // idx=>pos
        p = Hilbert2d::pos(i, order);
        // pos->idx
        int idx = Hilbert2d::index(p, order);
        // verify
        if (idx !=i )
        {
            printf("Verify fail at 2D, order %d!\n", order);
            return;
        }
    }
    printf("Verify pass at 2D, order %d\n", order);
}


void verify_3d(int order)
{
    int w = 1<<order; // width
    int n = w*w*w;
    for (int i=0; i<n; i++)
    {
        Point<3> p;
        // idx=>pos
        p = Hilbert3d::pos(i, order);
        // pos->idx
        int idx = Hilbert3d::index(p, order);
        // verify
        if (idx !=i )
        {
            printf("Verify fail at 3D, order %d!\n", order);
            return;
        }
    }
    printf("Verify pass at 3D, order %d\n", order);
}


int main (int argc, const char * argv[])
{
    printf("press enter to continue\n");
    show_idx_2d(1);
    getchar();
    show_idx_2d(2);
    getchar();
    show_idx_2d(3);
    getchar();
    show_pos_2d(1);
    getchar();
    show_pos_2d(2);
    getchar();
    show_pos_2d(3);
    getchar();
    printf("---------------\n");
    show_idx_3d(1);
    getchar();
    show_idx_3d(2);
    getchar();
    show_pos_3d(1);
    getchar();
    show_pos_3d(2);
    
    verify_2d(2);
    verify_2d(3);
    verify_2d(4);
    
    verify_3d(1);
    verify_3d(2);
    verify_3d(3);
    verify_3d(4);
    verify_3d(5);
    verify_3d(6);
    verify_3d(7);
    return 0;
}

