#pragma once

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

#include "max.h"

void MakeQuad(Face *f, int a, int b , int c , int d, int sg);
BITMAPINFO* AllocDIB( int bmw, int bmh );
void FreeDIB( BITMAPINFO* bmi ); 
BITMAPINFO *BMToDIB(Bitmap* newBM); 
BITMAPINFO* BitmapToDIB(Bitmap *bm);