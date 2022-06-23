//**************************************************************************/
// Copyright (c) 1998-2008 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
#pragma once

#include <WTypes.h>
#include "MaxNetExport.h"
#include "../maxnet_types.h"
#include "../maxheap.h"
#include "../strbasic.h"

/*! \sa  Class MaxNetManager, ::maxnet_error_t\n\n
\par Description:
The MaxNet class serves as the base class for MaxNetManager and should be
treated as the exception handler when using the Network Rendering API.  */
class MAXNETEXPORT MaxNet : public MaxHeapOperators
{
	protected:
		int	gerror;
		maxnet_error_t	error;
		maxnet_error_t	TranslateError	(int err);
	public:
						/*! \remarks Constructor */
						MaxNet			();
		/*! \remarks This method returns the MaxNet error. See the list of
		MaxNet error codes for details. */
		maxnet_error_t	GetError		();
		/*! \remarks This method returns the MaxNet error. See the list of
		MaxNet error codes for details. */
		int				GetGError		();
		/*! \remarks This method returns the MaxNet error description string.
		*/
		const MCHAR*	GetErrorText	();
};
