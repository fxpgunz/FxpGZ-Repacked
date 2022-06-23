//**************************************************************************/
// Copyright (c) 1998-2008 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/

#pragma once

#include "utilexp.h"

namespace MaxSDK
{
	namespace DebuggingTools
	{
		/*! A method to call a debug break during runtime.
		 * If called while running a release build, it will throw an exception and generate a Crash Error Report. 
		 * If called while running a debug build under the debugger, it will pause execution 
		 */
		UtilExport void DebugBreak();
	}
}