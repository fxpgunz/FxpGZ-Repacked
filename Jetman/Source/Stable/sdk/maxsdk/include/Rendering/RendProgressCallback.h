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
#include "../3dsmaxconfig.h"
#include "../strbasic.h"

/// \defgroup Values_returned_from_Progress__ Values Returned from Progress()
//@{
#define RENDPROG_CONTINUE	1	//!< Continue to Process
#define RENDPROG_ABORT		0	//!< Stop Processing
//@}

/// \defgroup Values_passed_to_SetCurField__ Values Passed to SetCurField()
//@{
#define FIELD_FIRST		0
#define FIELD_SECOND	1
#define FIELD_NONE		-1
//@}

/*! This class is a callback passed in to the renderer. The system passes this
callback to the renderer, and the renderer will use these methods whenever it
is doing something that is going to take some time. For instance when
transforming objects it can update the progress bar. This is also passed in to
the shadow buffer code so the shadow buffer can show its progress. All methods
of this class are implemented by the system. They are called by a plug-in
renderer.  
\see  Class Renderer.*/
class RendProgressCallback: public MaxHeapOperators
{
public:
	/*! Destructor. */
	virtual ~RendProgressCallback() { }
	/*! Allows the plug-in to set the string displayed in renderer
	dialog.
	\par Parameters:
	<b>const MCHAR *title</b>\n\n
	The string to display. Multiple title strings can be provided by separating them with newline ('\n') characters. The progress callback is then
    responsible for displaying these strings correctly, e.g. by alternating them every few seconds. */
	virtual void SetTitle(const MCHAR *title)=0;
	/*! Allows the plug-in to update the renderer progress display.
	\par Parameters:
	<b>int done</b>\n\n
	The number of items completed so far.\n\n
	<b>int total</b>\n\n
	The total number of items to process. For undeterminate (infinite render,...) progress, passing -1 as the total will trigger the marquee mode of the progress bar.
	\return  <b>RENDPROG_CONTINUE</b>\n\n
	Continue to process.\n\n
	<b>RENDPROG_ABORT</b>\n\n
	Stop processing. */
	virtual int Progress(int done, int total)=0;
	/*! Sets the field number display.
	\par Parameters:
	<b>int which</b>\n\n
	<b>FIELD_FIRST</b>\n\n
	<b>FIELD_SECOND</b>\n\n
	<b>FIELD_NONE</b> */
	virtual void SetCurField(int /*which*/) {}
	/*! The plug-in renderer should call this on every frame, passing
	in values for the various parameters. These are displayed in the rendering
	in progress dialog.
	\par Parameters:
	<b>int nlights</b>\n\n
	The total number of lights.\n\n
	<b>int nrayTraced</b>\n\n
	The number of lights using raytraced shadows.\n\n
	<b>int nshadowed</b>\n\n
	The number of lights using shadows.\n\n
	<b>int nobj</b>\n\n
	The total number of objects.\n\n
	<b>int nfaces</b>\n\n
	The total number of faces. */
	virtual void SetSceneStats(int /*nlights*/, int /*nrayTraced*/, int /*nshadowed*/, int /*nobj*/, int /*nfaces*/) {}

    /*! Sets the current rendering process step. This is used by the logging to give a global picture of the rendering process progress, not just of the current task.
    Let's say your rendering process is two steps (Translation and rendering) You would call this function at the beginning of the translation with (1,2)
    as parameters. Then use the Progress function as usual. Calling SetStep(2,2) at the beginning of the rendering task. This enable 3ds Max to know the
    total progress of the rendering task.
    \par Parameters:
    <b>int current</b>\n\n
    One based current step number. Passing zero or a negative value here will make the application skip the progress logging.
    <b>int total</b>\n\n
    One based total step count. Passing zero, negative value or a value lower than current will make the application skip the progress logging*/
    virtual void SetStep(int current, int total) = 0;
};

//! \deprecated in 3ds Max 2017. All methods of RendProgressCallback2 have been moved to class RendProgressCallback.
typedef MAX_DEPRECATED RendProgressCallback RendProgressCallback2;

