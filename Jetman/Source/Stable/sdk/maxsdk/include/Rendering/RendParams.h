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
#include "../RendType.h"
#include "../MaxTypes.h"
#include <windows.h>

class IRenderElementMgr;
class Texmap;
class Atmospheric;
class Effect;
class RadiosityEffect;
class ToneOperator;

#define FIELD_EVEN 0
#define FIELD_ODD 1

/// \defgroup extraFlags_field_of_RendParams Extra Flags Field of RendParams
//@{
/** Removes a fix made for R3 that  cleaned up the edges of shadows where objects intersect-- DS 8/28/00
* This goes in the extraFlags field of RendParams.*/
#define RENDER_R25SHADOWS    		  (1L << 2)
/** Tell the renderer to hide frozen objects
* This goes in the extraFlags field of RendParams. */
#define RENDER_HIDE_FROZEN    		  (1L << 3)
//@}

// These parameters are passed to the renderer when the renderer is opend.
/*!
This class has a set of data members, and these parameters are passed to the
renderer when the renderer is opened. All methods of this class are implemented
by the system.
\par Data Members:
<b>RendType rendType;</b>\n\n
The type of rendering to perform. \n\n
<b>BOOL isNetRender;</b>\n\n
Determines if this is a render on a network slave.\n\n
<b>BOOL fieldRender;</b>\n\n
If TRUE the image will be field rendered; otherwise frame rendered.\n\n
<b>int fieldOrder;</b>\n\n
The field order used. One of the following values:\n\n
<b>0</b> specifies even.\n\n
<b>1</b> specifies odd.\n\n
<b>TimeValue frameDur;</b>\n\n
This is used, for example, by video post. In video post you can stretch time. A
video post frame might be 1/2 frame long for example. This data member defines
the duration of one frame in TimeValue units.\n\n
<b>BOOL colorCheck;</b>\n\n
Determines if the color is ranged checked.\n\n
<b>int vidCorrectMethod;</b>\n\n
Video correction method. One of the following values:\n\n
<b>0</b> specifies FLAG (with black).\n\n
<b>1</b> specifies SCALE_LUMA (scale luminance).\n\n
<b>2</b> specifies SCALE_SAT (scale saturation).\n\n
<b>int ntscPAL;</b>\n\n
Determines if the color is range checked using NTSC or PAL standards. One of
the following values:\n\n
<b>0</b> specifies NTSC.\n\n
<b>1</b> specifies PAL.\n\n
<b>BOOL superBlack;</b>\n\n
If TRUE Super Black is used.\n\n
<b>int sbThresh;</b>\n\n
Specifies the Super Black threshold.\n\n
<b>BOOL rendHidden;</b>\n\n
If TRUE hidden objects are rendered.\n\n
<b>BOOL force2Side;</b>\n\n
If TRUE two sided materials are used for all items in the scene.\n\n
<b>BOOL inMtlEdit;</b>\n\n
If TRUE the rendering is taking place in the material editor.\n\n
<b>float mtlEditTile;</b>\n\n
If rendering is taking place in the material editor, scale tiling.\n\n
<b>BOOL mtlEditAA;</b>\n\n
If TRUE antialiasing should be done in the material editor.\n\n
<b>BOOL multiThread;</b>\n\n
This is used internally.\n\n
<b>BOOL useEnvironAlpha;</b>\n\n
If TRUE one should use alpha from the environment map.\n\n
<b>BOOL dontAntialiasBG;</b>\n\n
If the low-order bit is set don't antialias against the background (this is
often used for 'sprites' in video games). For 3ds Max 1.1 and 1.2 (in 2.0 and
later see <b>scanBandHeight</b> below), this parameter may also be used to
access the height of the abuffer in scan lines. This may be obtained using the
following syntax:
\code
abufBandHeight = rendpar.dontAntialiasBG>>8;
\endcode 
<b>BOOL useDisplacement;</b>\n\n
The apply displacement mapping setting.\n\n
<b>bool useRadiosity;</b>\n\n
Indicates if radiosity should be included in rendering.\n\n
<b>bool computeRadiosity;</b>\n\n
Indicates if radiosity should be computed before rendering.\n\n
<b>Texmap *envMap;</b>\n\n
The environment map. This may be NULL.\n\n
<b>Atmospheric *atmos;</b>\n\n
The atmosphere effects. This may be NULL. To the renderer it looks like there
is only one atmosphere. You can use this atmosphere and it will go ahead and
call all the individual atmospheric effects for you.\n\n
<b>Effect *effect;</b>\n\n
The post-processing effects. This may be NULL if there aren't any.\n\n
<b>RadiosityEffect* pRadiosity;</b>\n\n
Points to the radiosity effect.\n\n
<b>ToneOperator* pToneOp;</b>\n\n
Points to the tone operator if present. This may be NULL\n\n
<b>MCHAR biFileName[MAX_PATH];</b>\n\n
The bitmap output file name.\n\n
<b>TimeValue firstFrame;</b>\n\n
This is the first frame that will be rendered. This lets Open() know the
first frame that will be rendered, so it will not have to evaluate at frame 0
when building.\n\n
<b>int scanBandHeight;</b>\n\n
This is the height of a scan band (for the default 3ds Max scanline
renderer).\n\n
<b>ULONG extraFlags;</b>\n\n
<b>RENDER_HIDE_FROZEN</b>\n\n
Instruct the renderer to hide frozen objects\n\n
<b>int width;</b>\n\n
The image height.\n\n
<b>int height;</b>\n\n
The image width.\n\n
<b>BOOL filterBG;</b>\n\n
The filter background.  
\see  Class Atmospheric, Class Effect, Class IRenderElementMgr, Class ToneOperator.
*/
class RendParams: public MaxHeapOperators
{
private:
	IRenderElementMgr *mpIRenderElementMgr; // The render element manager, may be NULL -- declared in maxsdk/include/RenderElements.h

public:

	/** List of commands that can be passed to Execute()*/
	enum Execute_Commands {
		/** Queries whether this is a tone operator preview rendering. 
		 * Execute() will return non-zero if it is indeed a preview render. None of the "arg" parameters are used.*/
		kCommand_IsToneOpPreview = 0x4e80014c
	};

	RendType rendType;	 	// normal, region, blowup, selection

	BOOL isNetRender;  		// is this a render on a network slave?	
	BOOL fieldRender;
	int fieldOrder;    		// 0->even, 1-> odd
	TimeValue frameDur; 	// duration of one frame
	
	BOOL colorCheck;
	int vidCorrectMethod; 	// 0->FLAG, 1->SCALE_LUMA 2->SCALE_SAT
	int ntscPAL;  			// 0 ->NTSC,  1 ->PAL
	BOOL superBlack;		// impose superBlack minimum intensity?
	int sbThresh;			// value used for superBlack
	BOOL rendHidden;		// render hidden objects?
	BOOL force2Side;		// force two-sided rendering?
	BOOL inMtlEdit;	  		// rendering in the mtl editor?
	float mtlEditTile; 		// if in mtl editor, scale tiling by this value
	BOOL mtlEditAA;   		// if in mtl editor, antialias? 
	BOOL multiThread; 		// for testing only
	BOOL useEnvironAlpha;  	// use alpha from the environment map.
	BOOL dontAntialiasBG; 	// Don't antialias against background (for video games)		
	BOOL useDisplacement; 	// Apply displacement mapping		
	bool useRadiosity;		// Include radiosity in rendering
	bool computeRadiosity;	// Compute radiosity before rendering
	Texmap *envMap;			// The environment map, may be NULL
	Atmospheric *atmos; 	// The atmosphere effects, may be NULL.
	Effect *effect; 	    // The post-processing effects, may be NULL.
	RadiosityEffect* pRadiosity;	// The radiosity effect
	ToneOperator* pToneOp;	// The tone operator, may be NULL
	TimeValue firstFrame; 	// The first frame that will be rendered
	int scanBandHeight;		// height of a scan band (default scanline renderer)
	ULONG extraFlags;		// for expansion
	int width,height;		// image width,height.
	BOOL filterBG;			// filter background
	BOOL alphaOutOnAdditive;// produce alpha on additive transparency
	bool simplifyAreaLights;

	/*! Constructor. The initialization can be seen in the header
	file RENDER.H. */
	RendParams()
	{
		rendType = RENDTYPE_NORMAL;
		isNetRender = FALSE;
		fieldRender = FALSE;
		fieldOrder = 0;
		frameDur = 0;
		colorCheck = 0;
		vidCorrectMethod = 0;
		ntscPAL = 0;
		superBlack = 0;
		sbThresh = 0;
		rendHidden = 0;
		force2Side = 0;
		inMtlEdit = 0;
		mtlEditTile = 0;
		mtlEditAA = 0;
		multiThread = 0;
		useEnvironAlpha = 0;
		dontAntialiasBG = 0;
		useDisplacement = 0;
		useRadiosity = true;
		computeRadiosity = true;
		envMap = NULL;
		atmos = NULL;
		mpIRenderElementMgr = NULL;
		effect = NULL;
		pRadiosity = NULL;
		pToneOp = NULL;
		firstFrame = 0;
		scanBandHeight = 0;
		extraFlags = 0;
		width=height = 0;
		filterBG = 0;
		simplifyAreaLights = false;
	}

	/*! Destructor. */
	virtual ~RendParams() {}

	/*! \deprecated Removed in Max 2017; this method never worked correctly. */
	MAX_DEPRECATED int GetRenderMode();
	/*! Sets the render element manager used.
	\par Parameters:
	<b>IRenderElementMgr *pIRenderElementMgr</b>\n\n
	Points to the render element manager to set. */
	void SetRenderElementMgr(IRenderElementMgr *pIRenderElementMgr) { mpIRenderElementMgr = pIRenderElementMgr; }

#define RP_ANTIALIAS_OFF 200 //!< An index to an external command. Can be accepted as the argument to Execute() 

	/*! Returns a pointer to the render element manager interface. */
	IRenderElementMgr *GetRenderElementMgr() const { 
		return( const_cast<RendParams&>(*this).Execute(RP_ANTIALIAS_OFF) ? NULL : mpIRenderElementMgr); 
	}

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
	virtual INT_PTR Execute(int /*cmd*/, ULONG_PTR /*arg1*/=0, ULONG_PTR /*arg2*/=0, ULONG_PTR /*arg3*/=0) { return 0; }

	// Call this function to determine whether we are doing a tone operator preview rendering
	bool IsToneOperatorPreviewRender() const;
};

inline bool RendParams::IsToneOperatorPreviewRender() const {

	// Execute the special command on the render params
	INT_PTR result = const_cast<RendParams&>(*this).Execute(kCommand_IsToneOpPreview);
	return (result != 0);
}
