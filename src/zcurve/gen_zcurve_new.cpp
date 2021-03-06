#include <vector>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include "zcurve.h"

using namespace zcurve;
using namespace std;

template <int_fast8_t D>
void show(int L_max)
{
	int i;
	for (i=0; i<pow(2,L_max*D); i++)
	{
		auto p = convertZToPoint<int,D>(i);
		printf("%d => ", i);
		for (int j=0; j<D; j++)
			printf("%d ", p[j]);
		printf(" => %d\n", convertPointToZ<int,D>(p, pow(2,L_max)));
	}
		
}

struct Elem
{
	int id;
	long long  order;
	bool operator< (const Elem &x) const { return order < x.order; }
};

int main(int argc, const char **argv)
{
	printf("Usage: gen_zcurve x y z t\n");
	int dim[4];
	dim[0] = atoi(argv[1]);
	dim[1] = atoi(argv[2]);
	dim[2] = atoi(argv[3]);
	dim[3] = atoi(argv[4]);
	int max_dim = max(dim[0], max(dim[1], max(dim[2], dim[3])));
	int w = (int)pow(2, ceil(log((double)max_dim)/log(2.)));


	int total = dim[0]*dim[1]*dim[2]*dim[3];
	Elem *ary = new Elem[total];	
	int i;
	for (i=0; i<total; i++)
	{
		std::array<long long, 4> p;
		p[0] = i % dim[0];
		p[1] = (i / dim[0]) % dim[1];
		p[2] = (i / dim[0] / dim[1]) % dim[2];
		p[3] = i / dim[0] / dim[1] / dim[2];
		ary[i].id = i;
		ary[i].order = convertPointToZ<long long, 4>(p, w);
		//printf("%lld %lld %lld %lld=%lld\n", p[0], p[1], p[2], p[3], ary[i].order);
	}

	// sort the order
	sort(ary, ary+total);

	// now the array saves pos:id

	FILE *fp = fopen("out_zcurve.txt", "wt");
	fprintf(fp, "%d\n", total);
	for (i=0; i<total; i++)
	{
		fprintf(fp, "%d ", ary[i].id);
	}
	fclose(fp);

	return 0;
}
