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

#pragma warning(push)
#pragma warning(disable:4100)

#include "../BaseInterface.h"
#include "../Color.h"
#include "../Point2.h"

/*! This is passed to the renderer on every frame. This provides information about
the ambient light color, the background color, the duration of one frame, etc.
\par Data Members:
<b>Color ambient;</b>\n\n
The ambient light color.\n\n
<b>Color background;</b>\n\n
The background color.\n\n
<b>Color globalLightLevel;</b>\n\n
This is a multiplier that scales the brightness of all scene lights: it doesn't
affect the ambient light level.\n\n
<b>float frameDuration;</b>\n\n
The duration of one frame in units of current frames. This describes how much
scene time is used by one (video) frame time. For instance, in Video Post, you
can scale time so it's stretched out or compressed. In this case, you may be
rendering one frame to video, but because the scene is being run at a faster
speed than normal, you in fact see say 2.5 frames of the scene. Things such as
field rendering or motion blur must know about this so they know how long a
frame is in terms of the time of the scene.\n\n
<b>float relSubFrameDuration;</b>\n\n
This is the relative fraction of <b>frameDuration</b> used by a subframe.
Within the <b>frameDuration</b>, if you're sub-dividing the rendering up into
multiple frames for scene motion blur, this may be used.\n\n
For instance, say the duration (frames) is set to 0.5 and the duration
subdivisions is 5, then this data member would be 0.1. This means that each
subframe is effectively covering only 1/10th of the frame duration.\n\n
This value is always less than 1.0.\n\n
<b>int regxmin, regxmax;</b>\n\n
The x min and max boundary values for render region or render blowup in device
coordinates.\n\n
<b>int regymin, regymax;</b>\n\n
The y min and max boundary values for render region or render blowup in device
coordinates.\n\n
<b>Point2 blowupCenter;</b>\n\n
The 2D point at the center of the render blowup region.\n\n
<b>Point2 blowupFactor;</b>\n\n
The X and Y scale factors for render blowup.  
\see  Class RendParams, Class Color,  Class Point2.*/
class FrameRendParams : public BaseInterfaceServer {
public:
	Color ambient;
	Color background;
	Color globalLightLevel;
	float frameDuration; // duration of one frame, in current frames
	float relSubFrameDuration;  // relative fraction of frameDuration used by subframe.

	// boundaries of the region for render region or crop (device coords).
	int regxmin,regxmax;
	int regymin,regymax;

	// parameters for render blowup.
	Point2 blowupCenter;
	Point2 blowupFactor;

	/*! Constructor. The <b>frameDuration</b> is set to 1.0 and
	<b>relSubFrameDuration = 1.0f</b>. */
	FrameRendParams() { frameDuration = 1.0f; relSubFrameDuration = 1.0f; }
	/*! This is a general purpose function that allows the API to be extended in the
	future. The 3ds Max development team can assign new <b>cmd</b> numbers and
	continue to add functionality to this class without having to 'break' the API.
	\par Parameters:
	<b>int cmd</b>\n\n
	The index of the command to execute.\n\n
	<b>ULONG_PTR arg1=0</b>\n\n
	Optional argument 1. See the documentation where the <b>cmd</b> option is
	discussed for more details on these parameters.\n\n
	<b>ULONG_PTR arg2=0</b>\n\n
	Optional argument 2.\n\n
	<b>ULONG_PTR arg3=0</b>\n\n
	Optional argument 3.
	\return  An integer return value. See the documentation where the <b>cmd</b>
	option is discussed for more details on the meaning of this value. */
	virtual INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0; } 
};

#pragma warning(pop)