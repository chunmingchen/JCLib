#include <math.h>
#include <stdio.h>
#include "zcurve.h"

using namespace zcurve;

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
		printf(" => %d\n", convertPointToZ<int,D>(p, L_max));
	}
		
}


int main(int argc, const char **argv)
{
int i;
show<2>(2);
printf("\n");
show<4>(1);

return 0;
}
