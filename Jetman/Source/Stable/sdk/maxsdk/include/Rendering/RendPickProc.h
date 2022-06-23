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

class INode;

/*! An instance of this class is passed to IRendParams::SetPickMode(). This
is a callback that gets called as the user tries to pick objects in the scene.
 \see  Class IRendParams.
 */
class RendPickProc: public MaxHeapOperators
{
public:
	/*! Destructor. */
	virtual ~RendPickProc() { }

	/*! Called when the user picks something.
	\par Parameters:
	<b>INode *node</b>\n\n
	The node that was selected.
	\return  TRUE to end the pick mode; FALSE to continue. */
	virtual BOOL Pick(INode *node)=0;

	/*! Return TRUE if this is an acceptable hit; otherwise FALSE.
	\par Parameters:
	<b>INode *node</b>\n\n
	The node that was selected. */
	virtual BOOL Filter(INode *node)=0;

	/*! This method is called as the mode is entered.
	\par Default Implementation:
	<b>{}</b> */
	virtual void EnterMode() {}
	/*! This method is called when the mode is exited.
	\par Default Implementation:
	<b>{}</b> */
	virtual void ExitMode() {}

	/*! Returns the handle of the default cursor. This is the cursor to use when
	the user is not over a pickable object.
	\par Default Implementation:
	<b>{return NULL;}</b> */
	virtual HCURSOR GetDefCursor() {return NULL;}
	/*! Returns the handle of the hit cursor. This is the cursor to use when the
	user IS over a pickable object.
	\par Default Implementation:
	<b>{return NULL;}</b> */
	virtual HCURSOR GetHitCursor() {return NULL;}

	/*! \remarks	Implement this method to return TRUE to allow the user to pick more than
	one thing. In that case the Pick() method may be called more than
	once.
	\return  TRUE to allow multiple picks; otherwise FALSE.
	\par Default Implementation:
	<b>{return FALSE;}</b> */
	virtual BOOL AllowMultiSelect() {return FALSE;}
};
