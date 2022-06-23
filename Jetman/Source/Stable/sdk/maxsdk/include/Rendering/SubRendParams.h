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

#include "../BaseInterface.h"
#include "../RendType.h"
#include "../Point2.h"

/*! This structure contains information on rendering for Mirror and 
Automatic Cubic materials. This is used by the methods of the RenderMapsContext class.
\see Class RenderMapsContext.
*/
struct SubRendParams : public BaseInterfaceServer
{
	/*! The rendering type being done.*/
	RendType rendType;
	/*! TRUE if field rendering is being used; otherwise FALSE. */
	BOOL fieldRender;
	/*! This is used when field rendering. TRUE if doing even numbered scanlines; FALSE for odd numbered. */
	BOOL evenLines; 
	/*! This is used as part of implementing the Mirror material. It should be FALSE in all other cases. */
	BOOL doingMirror;
	BOOL doEnvMap;  // do environment maps?
	/*! The dimensions in pixels of Bitmap tobm. */
	int devWidth, devHeight;
	/*! The aspect ratio of Bitmap tobm. */
	float devAspect;
	/*! The location on the screen of the upper left corner of the output bitmap. */
	int xorg, yorg;
	/*! The area of the screen being rendered. */
	int xmin,xmax,ymin,ymax;

	/*! The 2D point at the center of the render blowup region. */
	Point2 blowupCenter;
	/*! The X and Y scale factors for render blowup. */
	Point2 blowupFactor;

	/*! \remarks	This is a general purpose function that allows the API to be extended in the
	future. The 3ds max development team can assign new cmd numbers and continue
	to add functionality to this class without having to 'break' the API.
	\par Parameters:
	<b>int cmd</b>\n
	The index of the command to execute.\n\n
	<b>ULONG arg1=0</b>\n
	Optional argument 1. See the documentation where the cmd option is discussed for more details on these parameters.\n\n
	<b>ULONG arg2=0</b>\n
	Optional argument 2.\n\n
	<b>ULONG arg3=0</b>\n
	Optional argument 3.
	\par Return Value:
	An integer return value. See the documentation where the cmd option is
	discussed for more details on the meaning of this value.
	*/
	virtual INT_PTR Execute(int /*cmd*/, ULONG_PTR /*arg1*/=0, ULONG_PTR /*arg2*/=0, ULONG_PTR /*arg3*/=0) { return 0; }
};