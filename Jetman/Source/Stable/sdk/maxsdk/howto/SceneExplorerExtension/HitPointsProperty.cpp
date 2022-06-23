// Copyright 2010 Autodesk, Inc.  All rights reserved.
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may not
// be disclosed to, copied or used by any third party without the prior written
// consent of Autodesk, Inc.
#pragma unmanaged
#include "HitPointsProperty.h"
#include <inode.h>
#include <strclass.h>

#pragma managed
#using <MaxExplorerBindings.dll>

using namespace SceneExplorer;

namespace SceneExplorerExtension
{

HitPointsProperty::HitPointsProperty() :
	INodePropertyDescriptor(GetName(), GetName(), PropertySettings::Undoable),
	mUserPropertyKey(new MSTR(_M("HitPoints")))
{ }

HitPointsProperty::~HitPointsProperty()
{ 
	delete mUserPropertyKey;
	mUserPropertyKey = nullptr;
}

System::Object^ HitPointsProperty::DoINodeGetValue(INode *node)
{
	int hitPoints = -1;
	if( node->GetUserPropInt(*mUserPropertyKey, hitPoints) )
	{
		return hitPoints;
	}
	else
	{
		return nullptr;
	}
}


void HitPointsProperty::DoINodeSetValue(INode* node, int value)
{
	node->SetUserPropInt(*mUserPropertyKey, value);
}

System::ComponentModel::PropertyDescriptor^ HitPointsProperty::Create()
{
	return gcnew HitPointsProperty();
}

System::String^ HitPointsProperty::GetName()
{
	return sPropertyName;
}


}