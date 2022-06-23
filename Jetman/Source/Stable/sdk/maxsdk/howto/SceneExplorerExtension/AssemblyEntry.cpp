// Copyright 2010 Autodesk, Inc.  All rights reserved.
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may not
// be disclosed to, copied or used by any third party without the prior written
// consent of Autodesk, Inc.
#pragma unmanaged
#include "AssemblyEntry.h"
#include "HitPointsProperty.h"

#pragma managed
#using <SceneExplorer.dll>
#using <ExplorerFramework.dll>

using namespace SceneExplorer;

namespace SceneExplorerExtension
{

void AssemblyEntry::AssemblyMain()
{
	// Create our Hit Points property and register it with the Scene Explorer
	NodePropertyRegistry::GetInstance()->AddProperty( HitPointsProperty::Create() );
}

void AssemblyEntry::AssemblyShutdown()
{
	// Remove the HitPoints property to ensure everything is properly cleaned up.
	// We didn't bother holding a reference to the property instance, so remove
	// it by name.
	NodePropertyRegistry::GetInstance()->RemoveProperty( HitPointsProperty::GetName() );
}

}
