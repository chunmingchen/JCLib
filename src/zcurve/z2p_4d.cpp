#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zcurve.h"

using namespace zcurve;

int main(int argc, const char **argv)
{
auto p= convertZToPoint<int, 4>(atoi(argv[1]) );
printf("%d %d %d %d\n", p[0] , p[1], p[2], p[3]);


return 0;
}
