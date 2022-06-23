// Copyright 2010 Autodesk, Inc.  All rights reserved.
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may not
// be disclosed to, copied or used by any third party without the prior written
// consent of Autodesk, Inc.

#pragma once

// Using #pragma managed(push, on) here and pragma managed(pop) at the end 
// explictly defines this header file as managed code without interfering with 
// the managed state at any site where this header is included.
#pragma managed(push, on)

namespace SceneExplorerExtension
{
// AssemblyEntry is a managed class (note the ref keyword) exporting 
// public, static AssemblyMain and AssemblyShutdown methods to define the
// entry and exit points for this managed plug-in. The AssemblyLoader.Loader
// will call the appropriate method as the plug-in is loaded or unloaded from 
// 3ds Max.
//
// After building, we must copy the plug-in to 3ds Max's bin\assemblies folder
// for the AssemblyLoader.Loader to find and load the plug-in.
//
// See the documentation for AssemblyLoader.Loader for more information.
public ref class AssemblyEntry
{
public:
	// Initialize this plug-in by registering our Hit Points property with the
	// Scene Explorer.
	static void AssemblyMain();
	
	// Remove the reference to our property from the Scene Explorer before
	// this plug-in is unloaded.
	static void AssemblyShutdown();
};

}
#pragma managed(pop)
