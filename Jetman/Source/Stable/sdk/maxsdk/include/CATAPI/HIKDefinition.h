//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2009 Autodesk, Inc.  All rights reserved.
//  Copyright 2003 Character Animation Technologies.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../MaxHeap.h"

// forward declarations
class INode;

// This struct should be modified to match the HIK definition.
// Pass an instance of this struct to CATParentTrans, and it will
// fill out its members
namespace CatAPI
{
	static const int MAX_CAT_SPINE_BONES = 9;

	struct HIKDefinition : public MaxHeapOperators
	{
		INode* mPelvis;

		// Left Leg.
		INode* mLeftThigh;
		INode* mLeftCalf;
		INode* mLeftAnkle;
		INode* mLeftToes[5][3];

		// Right Leg
		INode* mRightThigh;
		INode* mRightCalf;
		INode* mRightAnkle;
		INode* mRightToes[5][3];

		// Spine
		INode* mSpine[MAX_CAT_SPINE_BONES];
		INode* mRibcage;

		// Left Arm
		INode* mLeftCollar;
		INode* mLeftUpperArm;
		INode* mLeftForearm;
		INode* mLeftPalm;
		INode* mLeftFingers[5][3];

		// Right Arm
		INode* mRightCollar;
		INode* mRightUpperArm;
		INode* mRightForearm;
		INode* mRightPalm;
		INode* mRightFingers[5][3];

		// Head
		INode* mNeck[MAX_CAT_SPINE_BONES];
		INode* mHead;
	};
}