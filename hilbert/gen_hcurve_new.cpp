#include <vector>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include "hilbert.h"

using namespace std;

struct Elem
{
	int id;
	long long  order;
	bool operator< (const Elem &x) const { return order < x.order; }
};

int main(int argc, const char **argv)
{
	printf("Usage: gen_hcurve x y z\n");
	int dim[3];
	dim[0] = atoi(argv[1]);
	dim[1] = atoi(argv[2]);
	dim[2] = atoi(argv[3]);
	int max_dim = max(dim[0], max(dim[1], max(dim[2], 1)));
	int w = (int)ceil(log((double)max_dim)/log(2.));


	int total = dim[0]*dim[1]*dim[2]*1;
	Elem *ary = new Elem[total];	
	int i;
	for (i=0; i<total; i++)
	{
		if (dim[2]==1) // 2D
		{
			Point<2> p;
			p[0] = i % dim[0];
			p[1] = (i / dim[0]) % dim[1];
			//p[3] = i / dim[0] / dim[1] / 1;
			ary[i].id = i;
			ary[i].order = Hilbert<2>::index(p, w);
			//printf("%d %d %d =%lld\n", p[0], p[1], p[2], ary[i].order);

		} else {
			Point<3> p;
			p[0] = i % dim[0];
			p[1] = (i / dim[0]) % dim[1];
			p[2] = (i / dim[0] / dim[1]) ;
			//p[3] = i / dim[0] / dim[1] / 1;
			ary[i].id = i;
			ary[i].order = Hilbert<3>::index(p, w);
			//printf("%d %d %d =%lld\n", p[0], p[1], p[2], ary[i].order);
		}
	}

	// sort the order
	sort(ary, ary+total);

	// now the array saves pos:id

	FILE *fp = fopen("out_hcurve.txt", "wt");
	fprintf(fp, "%d\n", total);
	for (i=0; i<total; i++)
	{
		fprintf(fp, "%d ", ary[i].id);
	}
	fclose(fp);

	return 0;
}
