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


int main(int argc, const char **argv)
{
printf("Usage: gen_zcurve x y z t\n");
int dim[4];
dim[0] = atoi(argv[1]);
dim[1] = atoi(argv[2]);
dim[2] = atoi(argv[3]);
dim[3] = atoi(argv[4]);
printf("%d %d %d %d\n", dim[0], dim[1], dim[2], dim[3]);
int max_dim = max(max(max(dim[0], dim[1]), dim[2]), dim[3]);
int w = (int)pow(2, ceil(log((double)max_dim)/log(2.)));
long long dum_n = (long long)w*w*w*w;
printf("dum_n=%lld\n", dum_n);
vector<int> layout;

long long i;
for (i=0; i<dum_n; i++)
{
	
	auto p = convertZToPoint<long long,4>(i);
	swap(p[3], p[0]);
	if (p[0]<dim[0] && p[1]<dim[1] && p[2]<dim[2] && p[3]<dim[3])
		layout.push_back(p[0]+dim[0]*(p[1]+dim[1]*(p[2]+dim[2]*p[3])));

}
FILE *fp = fopen("out_zcurve.txt", "wt");
fprintf(fp, "%zu\n", layout.size());
for (i=0; i<layout.size(); i++)
{
	fprintf(fp, "%d ", layout[i]);
}
fclose(fp);

return 0;
}

/*

w=2^ceil(log(max(dim))/log(2))
layout=[];
count=0;
for i=1:w*w*w*w
        [dum,str_pos]=system(sprintf('./z2p_4d %d', i-1));
        pos=sscanf(str_pos, '%d %d %d %d');
        tmp=pos(1);
        pos(1)=pos(4);
        pos(4)=tmp;
        if pos(1)<dim(1) && pos(2)<dim(2) && pos(3)<dim(3) && pos(4)<dim(4)
                count=count+1;
                layout(count)=pos(1)+dim(1)*(pos(2)+dim(2)*(pos(3)+dim(3)*pos(4)));
        end
end
dlmwrite('out_zcurve.txt', [count layout], ' ')
*/
