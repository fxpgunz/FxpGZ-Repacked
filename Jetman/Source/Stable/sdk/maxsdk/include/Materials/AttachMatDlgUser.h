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
#include "../CoreExport.h"
#include <windows.h>

class IObjParam;

// Attach materials dialog support -- TH 3/9/99

// Attach Mat values
#define ATTACHMAT_IDTOMAT 0
#define ATTACHMAT_MATTOID 1
#define ATTACHMAT_NEITHER 2

/*! \sa  Class Object, Class Modifier.\n\n
\par Description:
This class provides a general way for objects to handle the attach materials
dialog presented when users attach objects to each other. To use this class do
the following:\n\n
1) Subclass your modifier or editable object off this class as shown below.
Implement the four functions that give access to the attach/condense
options.\n\n
2) Now your class simply calls the global <b>DoAttachMatOptionDialog()</b>
function, which deals with the parameters uniformly for all users. The
implementation in Edit Spline is like so:\n\n
\code
class EditSplineMod : public Modifier, ..., AttachMatDlgUser
{
	. . .
		static int attachMat;
	static BOOL condenseMat;
	. . .
}
// from AttachMatDlgUser
	int GetAttachMat() { return attachMat; }
	void SetAttachMat(int value) { attachMat = value; }
	BOOL GetCondenseMat() { return condenseMat; }
	void SetCondenseMat(BOOL sw) { condenseMat = sw; }
\endcode  
And the statics are defined as:\n\n
\code
int EditSplineMod::condenseMat = FALSE;
int EditSplineMod::attachMat = ATTACHMAT_NEITHER;
\endcode  */
class AttachMatDlgUser: public MaxHeapOperators {
	 public:
			/*! \remarks Destructor. */
			virtual ~AttachMatDlgUser() {;}
			/*! \remarks Returns the attach material setting. One of the following
			values:\n\n
			<b>ATTACHMAT_IDTOMAT</b>\n\n
			Match material IDs to material.\n\n
			<b>ATTACHMAT_MATTOID</b>\n\n
			Match Material to Material IDs.\n\n
			<b>ATTACHMAT_NEITHER</b>\n\n
			Do not modify Material IDs or Material. */
			virtual int GetAttachMat()=0;
			/*! \remarks Returns the condense material and IDs settings. */
			virtual BOOL GetCondenseMat()=0;
			/*! \remarks Sets the attach material value.
			\par Parameters:
			<b>int value</b>\n\n
			One of the following values:\n\n
			<b>ATTACHMAT_IDTOMAT</b>\n\n
			Match material IDs to material.\n\n
			<b>ATTACHMAT_MATTOID</b>\n\n
			Match Material to Material IDs.\n\n
			<b>ATTACHMAT_NEITHER</b>\n\n
			Do not modify Material IDs or Material. */
			virtual void SetAttachMat(int value)=0;
			/*! \remarks Sets the condense material and IDs setting.
			\par Parameters:
			<b>BOOL sw</b>\n\n
			TRUE for on; FALSE for off. */
			virtual void SetCondenseMat(BOOL sw)=0;
	 };

/*! \remarks This global function is available in release 3.0 and later
only.\n\n
This displays the dialog that you currently get in Edit(able) mesh when you
attach objects to each other. It gives options for how to deal with combining
materials, and whether or not to condense materials which have excess material
slots.
\par Parameters:
<b>IObjParam *ip</b>\n\n
The interface pointer.\n\n
<b>AttachMatDlgUser *user</b>\n\n
Points to the AttachMatDlgUser object.
\return  TRUE if the user OKed the dialog; FALSE if the user Cancelled.  */
CoreExport BOOL DoAttachMatOptionDialog(IObjParam *ip, AttachMatDlgUser *user);
