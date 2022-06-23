//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Helper functions
// AUTHOR: Jean-Francois Yelle, Devtech M&E
//***************************************************************************/

#include "helpers.h"

#include "bmmlib.h"
#include "bitmap.h"
#include "gamma.h"

void MakeQuad(Face *f, int a, int b , int c , int d, int sg) 
{
	f[0].setVerts( a, b, c);
	f[0].setSmGroup(sg);
	f[0].setEdgeVisFlags(1,1,0);
	f[1].setVerts( c, d, a);
	f[1].setSmGroup(sg);
	f[1].setEdgeVisFlags(1,1,0);
}


BITMAPINFO* AllocDIB( int bmw, int bmh ) 
{
	return (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + bmw * bmh * sizeof(RGBQUAD));
}
void FreeDIB( BITMAPINFO* bmi ) 
{
	if( bmi!=NULL ) free( bmi );
}

BITMAPINFO *BMToDIB(Bitmap* newBM)  
{
    int bmw = newBM->Width();
    int bmh = newBM->Height();
	BITMAPINFO* texBMI = AllocDIB( bmw, bmh );
	if( texBMI == NULL )
		return NULL;
    BITMAPINFOHEADER *bmih = &texBMI->bmiHeader;
    bmih->biSize = sizeof (BITMAPINFOHEADER);
    bmih->biWidth = bmw;
    bmih->biHeight = bmh;
    bmih->biPlanes = 1;
    bmih->biBitCount = 32;
    bmih->biCompression = BI_RGB;
    bmih->biSizeImage = bmih->biWidth * bmih->biHeight * sizeof(RGBQUAD);
    bmih->biXPelsPerMeter = 0;
    bmih->biYPelsPerMeter = 0;
    bmih->biClrUsed = 0;
    bmih->biClrImportant = 0;

    DWORD *row = (DWORD *)((BYTE *)texBMI + sizeof(BITMAPINFOHEADER));
    PixelBuf l64(bmw);

    for(int y = 0; y < bmh; y++)    {
        BMM_Color_64 *p64=l64.Ptr();
        // TBD: This should be GetLinearPixels, but since the viewport isn't 
        // yet gamma corrected,  do GetPixels instead.
        DWORD dw;

        int r,g,b;
        newBM->GetLinearPixels(0, y, bmw, p64);

        for(int x = 0; x < bmw; ++x, ++p64) {
            r = gammaMgr.disp_gamtabw[p64->r>>RCSH16];  
            g = gammaMgr.disp_gamtabw[p64->g>>RCSH16];  
            b = gammaMgr.disp_gamtabw[p64->b>>RCSH16];  
            dw = (r << 16) | (g << 8) | b ; 
            if (p64->a & 0x8000) 
                dw |= 0xff000000;
            row[x] = dw;
            }
        row += bmih->biWidth;
        }
    return texBMI;
}

BITMAPINFO* BitmapToDIB(Bitmap *bm) 
{	
    BITMAPINFO* texBMI = NULL;

    if(bm == NULL) 
        return NULL;

    texBMI = BMToDIB(bm);
    return texBMI;
}