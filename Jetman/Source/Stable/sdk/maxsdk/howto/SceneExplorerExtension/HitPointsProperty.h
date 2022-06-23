// Copyright 2010 Autodesk, Inc.  All rights reserved.
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may not
// be disclosed to, copied or used by any third party without the prior written
// consent of Autodesk, Inc.
#pragma once

#include <inode.h>
#include <strclass.h>
#pragma make_public(INode)

// Using #pragma managed(push, on) here and pragma managed(pop) at the end 
// explicitly defines this header file as managed code without interfering with 
// the managed state at any site where this header is included.
#pragma managed(push, on)

// Declare references to .NET assemblies.  Notice that it doesn't matter if
// an assembly is originally written in C++/CLI or C#, we use it same way.  
// The only difference from an external point of view is that C++/CLI may be
// aware of native types.

// SceneExplorer.dll is a 3ds Max .NET assembly written in C++/CLI.
#using <SceneExplorer.dll>
// ExplorerFramework is a 3ds Max .NET assembly written in C#.
#using <ExplorerFramework.dll>
// Microsoft .NET Framework assembly.
#using <System.dll>

namespace SceneExplorerExtension
{

// This is a managed class.  Note the 'ref' keyword.
public ref class HitPointsProperty : SceneExplorer::INodePropertyDescriptor<int>
{
public:
	// Factory method, builds the property with descriptors
	static System::ComponentModel::PropertyDescriptor^ Create();
	static System::String^ GetName();

	~HitPointsProperty();

protected:
	// implement the accessor operations
	virtual System::Object^ DoINodeGetValue(INode *node) override;
	virtual void DoINodeSetValue(INode* node, int value) override;

private:
	// Client code should use the factory method instead.
	HitPointsProperty();

	// For a managed class to have a native type as a data member, the native 
	// data member must be a reference or a pointer.
	MSTR* mUserPropertyKey;
	static System::String^ sPropertyName = gcnew System::String(_T("HitPoints"));
};

}

#pragma managed(pop)
