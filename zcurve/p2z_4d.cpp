#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zcurve.h"

using namespace zcurve;

int main(int argc, const char **argv)
{
std::array<long long, 4> p;
p[0] = atoi(argv[2]);
p[1] = atoi(argv[3]);
p[2] = atoi(argv[4]);
p[3] = atoi(argv[5]);
printf("%lld\n", convertPointToZ<long long, 4>(p, atoi(argv[1]) ));


return 0;
}
