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

#include "../AnimProperty.h"
#include "../MaxTypes.h"

class ILoad;
class ISave;

/*! \sa  Class AnimProperty.\n\n
\par Description:
This is used internally by the Material / Map Browser which supports the
display of small and large icon images for material and texture maps. This
class is the postage stamp image object.\n\n
The width in bytes of the image pixel array is given by the following macro,
where w is pixel width.\n\n
<b>\#define ByteWidth(w) (((w*3+3)/4)*4)</b>  */
class PStamp: public AnimProperty {
	 public:
	 /*! \remarks Returns the width of the image in pixels. */
	 virtual int Width()=0;
	 /*! \remarks Returns the height of the image in pixels. */
	 virtual int Height()=0;
	 /*! \remarks Sets the image for the postage stamp.
	 \par Parameters:
	 <b>UBYTE *img</b>\n\n
	 This is an array of RGB triplets. */
	 virtual void SetImage(UBYTE *img)=0;
	 /*! \remarks Retrieves the image bytes of the postage stamp.
	 \par Parameters:
	 <b>UBYTE *img</b>\n\n
	 This is an array of RGB triplets. */
	 virtual void GetImage(UBYTE *img)=0;
	 /*! \remarks This method is called to delete this instance of the class. */
	 virtual void DeleteThis()=0;
	 /*! \remarks This method is used to load the postage stamp image. */
	 virtual IOResult Load(ILoad *iload)=0;
	 /*! \remarks This method is used to save the postage stamp image. */
	 virtual IOResult Save(ISave *isave)=0;
};
