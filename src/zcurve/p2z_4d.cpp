#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zcurve.h"

using namespace zcurve;

int main(int argc, const char **argv)
{
fprintf(stderr, "width, x,y,z,w\n");
std::array<int, 4> p;
p[0] = atoi(argv[2]);
p[1] = atoi(argv[3]);
p[2] = atoi(argv[4]);
p[3] = atoi(argv[5]);
//fprintf(stderr, "%d %d %d %d\n", p[0], p[1], p[2], p[3]);
printf("%d\n", convertPointToZ<int, 4>(p, atoi(argv[1]) ));


return 0;
}
