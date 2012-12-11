//
//  hilbert.h
//  Hilbert
//
//  Created by Jimmy Chen on 12/3/11.
//  Copyright 2011 OSU. All rights reserved.
//

#ifndef Hilbert_hilbert_h
#define Hilbert_hilbert_h

template <int dim>
struct Point{int v[dim];};

extern int hilbert_pos2d[4][4];
extern int hilbert_dir2d[4][4];

extern int hilbert_pos3d[];
extern int hilbert_rot3d[8][9];
extern int hilbert_tx3d[8][3];
extern int hilbert_inv_rot3d[8][9];
extern int hilbert_inv_tx3d[8][3];
extern int hilbert_inv_pos3d[8][3];

template 
class Hilbert2d{
    enum {dim=2};
    // [x,y]=>idx
    static int index2d(int x, int y, int order) // width = 2^order
    {
        int cur_dir = 0;
        int pos = 0;
        int i;
        for (i=order-1; i>=0; i--)
        {
            pos <<=dim;
            int quad_x = (x&(1<<i)) > 0;
            int quad_y = (y&(1<<i)) > 0;
            int quad_pos = hilbert_pos2d[cur_dir][(quad_y<<1)|quad_x];
            cur_dir = hilbert_dir2d[cur_dir][(quad_y<<1)|quad_x];
            pos |= quad_pos;
        }
        return pos;
    }
    // idx=>[x,y]
    static Point<dim> hilbert_getPos2d(int idx, int order)
    {
        Point<dim> p;
        if (order==0) {
            p.v[0]=p.v[1]=0;
            return p;
        }
        p = hilbert_getPos2d(idx, order-1);
        int quad_idx = (idx>>(dim*(order-1))) & ((1<<dim)-1);
        int t=1<<((order-1));
        Point<dim> p1;
        switch (quad_idx) {
            case 0:
                p1.v[0]=p.v[1]-t;
                p1.v[1]=p.v[0]-t;
                break;
            case 1:
                p1.v[0]=p.v[0]+t;
                p1.v[1]=p.v[1]-t;
                break;
            case 2:
                p1.v[0]=p.v[0]+t;
                p1.v[1]=p.v[1]+t;
                break;
            case 3:
                p1.v[0]=-p.v[1]-t;
                p1.v[1]=-p.v[0]+t;
                break;
        };
        return p1;
    }    
public:
    // position to index
    static int index(Point<dim> p, int order)
    {
        return index2d(p.v[0], p.v[1], order);
    }    
    
    
    // index to position
    static Point<dim> pos(int idx, int order)
    {
        Point<dim> p = hilbert_getPos2d(idx, order);
        p.v[0] = (p.v[0]+ (1<<order) -1) >> 1;
        p.v[1] = (p.v[1]+ (1<<order) -1) >> 1;
        return p;
            
    }
};


class Hilbert3d{
    enum {dim=3};
    // [x,y,z]=>idx
    // method: top-down
    static int index3d(int x, int y, int z, int order) // width = 2^order
    {
        int i;
        int pos=0;
        for (i=order-1; i>=0; i--)
        {
            pos <<= dim;
            int quad_x = (x&(1<<i)) > 0;
            int quad_y = (y&(1<<i)) > 0;
            int quad_z = (z&(1<<i)) > 0;
            int quad_pos = hilbert_pos3d[(quad_z<<2)|(quad_y<<1)|quad_x];
            rot_tx_3d(quad_pos, x,y,z, order);
            pos |= quad_pos;
        }
        return pos;
    }
    
    static void rot_tx_3d(int pos, int &x, int &y, int &z, int order)
    {
        int tx=x,ty=y,tz=z;
        int *rot = hilbert_rot3d[pos];
        x = rot[0]*tx+rot[1]*ty+rot[2]*tz;
        y = rot[3]*tx+rot[4]*ty+rot[5]*tz;
        z = rot[6]*tx+rot[7]*ty+rot[8]*tz;
        x+=hilbert_tx3d[pos][0]* ((1<<order)-1);
        y+=hilbert_tx3d[pos][1]* ((1<<order)-1);
        z+=hilbert_tx3d[pos][2]* ((1<<order)-1);
    }
    static void inv_rot_tx_3d(int pos, int &x, int &y, int &z, int order)
    {
        int tx=x,ty=y,tz=z;
        int *rot = hilbert_inv_rot3d[pos];
        x = rot[0]*tx+rot[1]*ty+rot[2]*tz;
        y = rot[3]*tx+rot[4]*ty+rot[5]*tz;
        z = rot[6]*tx+rot[7]*ty+rot[8]*tz;
        x+=hilbert_inv_tx3d[pos][0]* ((1<<order)-1);
        y+=hilbert_inv_tx3d[pos][1]* ((1<<order)-1);
        z+=hilbert_inv_tx3d[pos][2]* ((1<<order)-1);
    }
    
    // idx=>[x,y,z]
    static Point<dim> hilbert_getPos3d(int idx, int order)
    {
        Point<dim> p;
        if (order==0) {
            p.v[0]=p.v[1]=p.v[2]=0;
            return p;
        }
        p = hilbert_getPos3d(idx, order-1);
        int quad_idx = (idx>>(dim*(order-1))) & ((1<<dim)-1);
        inv_rot_tx_3d(quad_idx, p.v[0], p.v[1], p.v[2], order-1);
#if 1
        int t=1<<(order-1);
        p.v[0]+=t * hilbert_inv_pos3d[quad_idx][0];
        p.v[1]+=t * hilbert_inv_pos3d[quad_idx][1];
        p.v[2]+=t * hilbert_inv_pos3d[quad_idx][2];
        return p;
#else
        int t=1<<((order));
        Point<dim> p1;
        switch (quad_idx) {
            case 0:
                p1.v[0]=p.v[2]-t;
                p1.v[1]=p.v[0]-t;
                p1.v[2]=p.v[1]-t;
                break;
            case 1:
                p1.v[0]=p.v[1]-t;
                p1.v[1]=p.v[2]-t;
                p1.v[2]=p.v[0]+t;
                break;
            case 2:
                p1.v[0]=p.v[1]+t;
                p1.v[1]=p.v[2]-t;
                p1.v[2]=p.v[0]+t;
                break;
            case 3:
                p1.v[0]=-p.v[0]+t;
                p1.v[1]=+p.v[1]-t;
                p1.v[2]=-p.v[2]-t;
                break;
            case 4:
                p1.v[0]=-p.v[0]+t;
                p1.v[1]=p.v[1]+t;
                p1.v[2]=-p.v[2]-t;
                break;
            case 5:
                p1.v[0]=-p.v[1]+t;
                p1.v[1]=-p.v[2]+t;
                p1.v[2]=p.v[0]+t;
                break;
            case 6:
                p1.v[0]=-p.v[1]-t;
                p1.v[1]=-p.v[2]+t;
                p1.v[2]=p.v[0]+t;
                break;
            case 7:
                p1.v[0]=p.v[2]-t;
                p1.v[1]=-p.v[0]+t;
                p1.v[2]=-p.v[1]-t;
                break;
        };
        return p1;
#endif
    }
    
    
public:
    // position to index
    static int index(Point<dim> p, int order)
    {
        return index3d(p.v[0], p.v[1], p.v[2], order);
    }    
    
    
    // index to position
    static Point<dim> pos(int idx, int order)
    {
        Point<dim> p = hilbert_getPos3d(idx, order);
        return p;
    }
    
};


#if 0
template <int dim>
class Hilbert{
    // [x,y]=>idx
    static int index2d(int x, int y, int order) // width = 2^order
    {
        int cur_dir = 0;
        int pos = 0;
        int i;
        for (i=order-1; i>=0; i--)
        {
            pos =pos<<dim;
            int quad_x = (x&(1<<i)) > 0;
            int quad_y = (y&(1<<i)) > 0;
            int quad_pos = hilbert_pos2d[cur_dir][(quad_y<<1)|quad_x];
            cur_dir = hilbert_dir2d[cur_dir][(quad_y<<1)|quad_x];
            pos |= quad_pos;
        }
        return pos;
    }
    // [x,y,z]=>idx
    // method: top-down
    static int index3d(int x, int y, int z, int order) // width = 2^order
    {
        int i;
        int pos=0;
        for (i=order-1; i>=0; i--)
        {
            pos <<= dim;
            int quad_x = (x&(1<<i)) > 0;
            int quad_y = (y&(1<<i)) > 0;
            int quad_z = (z&(1<<i)) > 0;
            int quad_pos = hilbert_pos3d[(quad_z<<2)|(quad_y<<1)|quad_x];
            rot_tx_3d(quad_pos, x,y,z, order);
            pos |= quad_pos;
        }
        return pos;
    }
    
    static void rot_tx_3d(int pos, int &x, int &y, int &z, int order)
    {
        int tx=x,ty=y,tz=z;
        int *rot = hilbert_rot3d[pos];
        x = rot[0]*tx+rot[1]*ty+rot[2]*tz;
        y = rot[3]*tx+rot[4]*ty+rot[5]*tz;
        z = rot[6]*tx+rot[7]*ty+rot[8]*tz;
        x+=hilbert_tx3d[pos][0]* ((1<<order)-1);
        y+=hilbert_tx3d[pos][1]* ((1<<order)-1);
        z+=hilbert_tx3d[pos][2]* ((1<<order)-1);
    }
    static void inv_rot_tx_3d(int pos, int &x, int &y, int &z, int order)
    {
        int tx=x,ty=y,tz=z;
        int *rot = hilbert_inv_rot3d[pos];
        x = rot[0]*tx+rot[1]*ty+rot[2]*tz;
        y = rot[3]*tx+rot[4]*ty+rot[5]*tz;
        z = rot[6]*tx+rot[7]*ty+rot[8]*tz;
        x+=hilbert_inv_tx3d[pos][0]* ((1<<order)-1);
        y+=hilbert_inv_tx3d[pos][1]* ((1<<order)-1);
        z+=hilbert_inv_tx3d[pos][2]* ((1<<order)-1);
    }
    
    // idx=>[x,y]
    static Point<dim> hilbert_getPos2d(int idx, int order)
    {
        Point<dim> p;
        if (order==0) {
            p.v[0]=p.v[1]=0;
            return p;
        }
        p = hilbert_getPos2d(idx, order-1);
        int quad_idx = (idx>>(dim*(order-1))) & ((1<<dim)-1);
        int t=1<<((order-1));
        Point<dim> p1;
        switch (quad_idx) {
            case 0:
                p1.v[0]=p.v[1]-t;
                p1.v[1]=p.v[0]-t;
                break;
            case 1:
                p1.v[0]=p.v[0]+t;
                p1.v[1]=p.v[1]-t;
                break;
            case 2:
                p1.v[0]=p.v[0]+t;
                p1.v[1]=p.v[1]+t;
                break;
            case 3:
                p1.v[0]=-p.v[1]-t;
                p1.v[1]=-p.v[0]+t;
                break;
        };
        return p1;
    }
    // idx=>[x,y,z]
    static Point<dim> hilbert_getPos3d(int idx, int order)
    {
        Point<dim> p;
        if (order==0) {
            p.v[0]=p.v[1]=p.v[2]=0;
            return p;
        }
        p = hilbert_getPos3d(idx, order-1);
        int quad_idx = (idx>>(dim*(order-1))) & ((1<<dim)-1);
        inv_rot_tx_3d(quad_idx, p.v[0], p.v[1], p.v[2], order-1);
#if 1
        int t=1<<(order-1);
        p.v[0]+=t * hilbert_inv_pos3d[quad_idx][0];
        p.v[1]+=t * hilbert_inv_pos3d[quad_idx][1];
        p.v[2]+=t * hilbert_inv_pos3d[quad_idx][2];
        return p;
#else
        int t=1<<((order));
        Point<dim> p1;
        switch (quad_idx) {
            case 0:
                p1.v[0]=p.v[2]-t;
                p1.v[1]=p.v[0]-t;
                p1.v[2]=p.v[1]-t;
                break;
            case 1:
                p1.v[0]=p.v[1]-t;
                p1.v[1]=p.v[2]-t;
                p1.v[2]=p.v[0]+t;
                break;
            case 2:
                p1.v[0]=p.v[1]+t;
                p1.v[1]=p.v[2]-t;
                p1.v[2]=p.v[0]+t;
                break;
            case 3:
                p1.v[0]=-p.v[0]+t;
                p1.v[1]=+p.v[1]-t;
                p1.v[2]=-p.v[2]-t;
                break;
            case 4:
                p1.v[0]=-p.v[0]+t;
                p1.v[1]=p.v[1]+t;
                p1.v[2]=-p.v[2]-t;
                break;
            case 5:
                p1.v[0]=-p.v[1]+t;
                p1.v[1]=-p.v[2]+t;
                p1.v[2]=p.v[0]+t;
                break;
            case 6:
                p1.v[0]=-p.v[1]-t;
                p1.v[1]=-p.v[2]+t;
                p1.v[2]=p.v[0]+t;
                break;
            case 7:
                p1.v[0]=p.v[2]-t;
                p1.v[1]=-p.v[0]+t;
                p1.v[2]=-p.v[1]-t;
                break;
        };
        return p1;
#endif
    }
    

public:
    // position to index
    static int index(Point<dim> p, int order)
    {
        if (dim==2) 
            return index2d(p.v[0], p.v[1], order);
        else if (dim==3)
        {
            return index3d(p.v[0], p.v[1], p.v[2], order);
        }else {
            printf("Dimension Not supported\n");
            return 0;
        }
        
    }    

    
    // index to position
    static Point<dim> pos(int idx, int order)
    {
        if (dim==2) {
            Point<dim> p = hilbert_getPos2d(idx, order);
            p.v[0] = (p.v[0]+ (1<<order) -1) >> 1;
            p.v[1] = (p.v[1]+ (1<<order) -1) >> 1;
            return p;
        } else if (dim==3) {
            Point<dim> p = hilbert_getPos3d(idx, order);
            return p;
        } else {
            printf("Dimension Not supported\n");
            return Point<dim>();
        }
        
    }
    
};
#endif


#endif
