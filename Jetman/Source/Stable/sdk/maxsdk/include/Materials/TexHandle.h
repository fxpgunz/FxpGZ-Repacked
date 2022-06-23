//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../MaxHeap.h"
#include <windows.h>

class Bitmap;

/*! \sa  Class TexHandleMaker.\n\n
\par Description:
This class defines a texture handle. A pointer to an instance of this class is
returned from the methods of <b>TexHandleMaker</b>. Methods of this class allow
the handle to be retrieved and to delete the handle.  */
class TexHandle: public MaxHeapOperators {
	 public:
			/*! \remarks Destructor. */
			virtual ~TexHandle() {;}
		/*! \remarks This method is called to retrieve the texture handle. */
			virtual DWORD_PTR GetHandle() = 0;
			/*! \remarks This method is called to delete the instance of the texture handle. */
			virtual void DeleteThis() = 0;
	 };    

// Values for extraFlags:
#define EX_MULT_ALPHA     1    // if alpha is NOT premultiplied in the Bitmap, set this 
#define EX_RGB_FROM_ALPHA 2    // make map using alpha to define gray level 
#define EX_OPAQUE_ALPHA   4    // make map using opaque alpha 
#define EX_ALPHA_FROM_RGB 8    // make alpha from intensity 

/*! \sa  Class TexHandle, Class Bitmap, Class BitmapInfo.\n\n
\par Description:
This class provides several ways to create a texture handle. The handle may be
created from a 3ds Max bitmap or a Windows Device Independent Bitmap. This
class also provides methods to determine the desired size of the bitmap. <br>
*/
class TexHandleMaker: public MaxHeapOperators{
	 public: 
			/*! \remarks Destructor. */
			virtual ~TexHandleMaker() {;}
			// choice of two ways to create a texture handle.
			// From a 3DSMax Bitmap
			/*! \remarks Implemented by the System\n\n
			This method is called to create a texture handle from a 3ds Max bitmap.
			\par Parameters:
			<b>Bitmap *bm</b>\n\n
			The bitmap to create a handle to.\n\n
			<b>int symflags=0</b>\n\n
			See \ref textureSymmetryFlags.\n\n
			<b>int extraFlags=0;</b>\n\n
			One of the following values:\n\n
			<b>EX_MULT_ALPHA</b>\n\n
			Set this flag if alpha is <b>not</b> pre-multiplied in the Bitmap.\n\n
			<b>EX_RGB_FROM_ALPHA</b>\n\n
			Set this flag to make the map using the alpha channel of the bitmap to
			define the gray level.\n\n
			<b>EX_OPAQUE_ALPHA</b>\n\n
			Specifies to make the map using opaque alpha.\n\n
			<b>EX_ALPHA_FROM_RGB</b>\n\n
			Specifies to make alpha from the intensity of the map.
			\return  A pointer to the texture handle. */
			virtual TexHandle* CreateHandle(Bitmap *bm, int symflags=0, int extraFlags=0)=0;

			// From a 32 bit DIB
			/*! \remarks Implemented by the System\n\n
			This method is called to create a texture handle from a 32 bit Windows
			Device Independent Bitmap.
			\par Parameters:
			<b>BITMAPINFO *bminf</b>\n\n
			The bitmap to create a handle to.\n\n
			<b>int symflags=0</b>\n\n
			See \ref textureSymmetryFlags.\n\n
			<b>int extraFlags=0;</b>\n\n
			One of the following values:\n\n
			<b>EX_MULT_ALPHA</b>\n\n
			Set this flag if alpha is <b>not</b> pre-multiplied in the Bitmap.\n\n
			<b>EX_RGB_FROM_ALPHA</b>\n\n
			Set this flag to make the map using the alpha channel of the bitmap to
			define the gray level.\n\n
			<b>EX_OPAQUE_ALPHA</b>\n\n
			Specifies to make the map using opaque alpha.\n\n
			<b>EX_ALPHA_FROM_RGB</b>\n\n
			Specifies to make alpha from the intensity of the map.
			\return  A pointer to the texture handle. */
			virtual TexHandle* CreateHandle(BITMAPINFO *bminf, int symflags=0, int extraFlags=0)=0;

			// Create an 32 bit DIB with symmetry, extraflags already incorporated.
			// iff forceW is non-zero, it is used as the width of the final DIB 
			// iff forceH is non-zero, it is used as the height of the final DIB 
			/*! \remarks			This method creates a 32 bit Windows Device Independent Bitmap with the
			specified symflags and extraflags already incorporated and returns a
			pointer to the associated BitmapInfo.
			\par Parameters:
			<b>Bitmap *bm</b>\n\n
			Points to the bitmap to create the handle to.\n\n
			<b>int symflags</b>\n\n
			See \ref textureSymmetryFlags.\n\n
			<b>int extraFlags</b>\n\n
			One of the following values:\n\n
			<b>EX_MULT_ALPHA</b>\n\n
			Set this flag if alpha is <b>not</b> pre-multiplied in the Bitmap.\n\n
			<b>EX_RGB_FROM_ALPHA</b>\n\n
			Set this flag to make the map using the alpha channel of the bitmap to
			define the gray level.\n\n
			<b>EX_OPAQUE_ALPHA</b>\n\n
			Specifies to make the map using opaque alpha.\n\n
			<b>EX_ALPHA_FROM_RGB</b>\n\n
			Specifies to make alpha from the intensity of the map.\n\n
			<b>BOOL forceW=0</b>\n\n
			If this parameter is non-zero it is used as the width of the final
			DIB.\n\n
			<b>BOOL forceH=0</b>\n\n
			If this parameter is non-zero it is used as the height of the final DIB
			*/
			virtual BITMAPINFO *BitmapToDIB(Bitmap *bm, int symflags, int extraFlags, BOOL forceW=0, BOOL forceH=0)=0;

			// From a 32 bit DIB that has symflags, extraflags already incorporated.
			/*! \remarks			Returns a texture handle made from a 32 bit DIB that has the symflags,
			and extraflags already incorporated. This takes ownership of the
			BITMAPINFO*.
			\par Parameters:
			<b>BITMAPINFO* bminf</b>\n\n
			Points to the BitmapInfo for the DIB. */
			virtual TexHandle* MakeHandle(BITMAPINFO*bminf)=0;  // this takes ownership of the BITMAPINFO*

			// This tells you the size desired of the bitmap. It ultimately
			// needs a bitmap that is a power of 2 in width and height.  If 
			// UseClosestPowerOf2 returns TRUE, then the bitmap need not be square.
			// If you already have a bitmap around, just pass it in to CreateHandle
			// and it will be converted.  If you are making a bitmap from scratch
			// (i.e.) a procedural texture, then you should make it Size() in 
			// width in height, and save us an extra step.  In either case
			// You own your bitmap, and are responsible for ultimately freeing it.

			/*! \remarks			Returns TRUE if the bitmap does not need to be square and FALSE if it
			does need to be square. */
			virtual BOOL UseClosestPowerOf2()=0;
			/*! \remarks Implemented by the System.\n\n
			This method may be called to determine the desired size for the bitmap.
			The system ultimately needs a square bitmap that is a power of 2 in width
			and height. If you already have a bitmap around, just pass it in to
			<b>CreateHandle()</b> and it will be converted. If you are creating a
			bitmap from scratch (i.e. a procedural texture), then you should make it
			<b>Size()</b> in width in height, and save the system an extra step. In
			either case you own your bitmap, and are responsible for ultimately
			freeing it.
			\return  The size of the desired bitmap. */
			virtual int Size()=0;
	 };
