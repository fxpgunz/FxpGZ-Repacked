//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// AUTHOR: Nicolas Desjardins
// DATE: 2007-01-08 
//***************************************************************************/

#pragma once
#pragma managed(push, on)

namespace ManagedServices
{

// Note that this documentation does not use "///" for extraction by the
// compiler.  It will fail to be processed correctly.  Here's Microsoft's
// explanation:

// In the current release, code comments are not processed on templates or 
// anything containing a template type (for example, a function taking a 
// parameter as a template). Doing so will result in undefined behavior.
// [Copied from web MSDN on 2007-08-31.]

// <summary>
// Interface formalizing accessors for classes wrapping data nodes.
// </summary>
// <remarks>
// Note that for documentation to work correctly, template classes must be 
// kept internal to their assemblies.
// </remarks>
template<typename HeldType_>
private interface class DataNodeWrapper
{
public:
	// <summary>
	// Type of the wrapped data item.
	// </summary>
	typedef HeldType_ HeldType;

	// <summary>
	// Get the data item held in this wrapper.
	// </summary>
	// <returns>
	// The data item held in this wrapper.
	// </returns>
	virtual HeldType GetDataNode();

	// <summary>
	// Get the Type of the data item held in this wrapper.
	// </summary>
	// <returns>
	// The Type of the data item held in this wrapper.
	// </returns>
	virtual System::Type^ GetDataNodeType();
};

}

#pragma managed(pop)
