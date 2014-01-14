#include <stdio.h>
#include <stdlib.h>
#include "nrrd.h"


bool write_nrrd_3d(const char *nrrd_fname, const char *raw_fname, int w, int h, int d, const char *type)
{

FILE *fp = fopen(nrrd_fname, "wt");
fprintf(fp, "NRRD0001\n"
"type: %s\n"
"dimension: 3\n"
"sizes: %d %d %d\n"
"encoding: raw\n"
"data file: %s\n",
type, w, h, d, raw_fname);

fclose(fp);
}

