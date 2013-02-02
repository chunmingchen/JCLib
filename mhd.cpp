#include <stdio.h>
#include <stdlib.h>
#include "mhd.h"


bool write_mhd_3d(const char *szFilename2, const char *raw_fname, int w, int h, int d, const char *format)
{

FILE *fp = fopen(szFilename2, "wt");
fprintf(fp, "ObjectType = Image\nNDims = 3\nBinaryData = True\nBinaryDataByteOrderMSB = False\nTransformMatrix = 1 0 0 0 1 0 0 0 1\nOffset = %d %d %d\nCenterOfRotation = 0 0 0\nElementSpacing = 1 1 1\nDimSize = %d %d %d\nAnatomicalOrientation = ???\nElementType = MET_%s\n", -w/2, -h/2, -d/2, w, h, d, format);

fprintf(fp, "ElementDataFile = %s\n", raw_fname);

fclose(fp);
}

