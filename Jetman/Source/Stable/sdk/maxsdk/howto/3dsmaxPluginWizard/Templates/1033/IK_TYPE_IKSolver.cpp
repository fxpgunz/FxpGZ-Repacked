//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "[!output PROJECT_NAME].h"

#define [!output CLASS_NAME]_CLASS_ID	Class_ID([!output CLASSID1], [!output CLASSID2])

using namespace IKSys;

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME]
{
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	virtual ~[!output CLASS_NAME]();

	virtual bool IsHistoryDependent() const;
	virtual bool DoesOneChainOnly() const;

	virtual bool IsInteractive() const;
	virtual bool UseSlidingJoint() const;
	virtual bool UseSwivelAngle() const;
	virtual bool IsAnalytic() const ;

	virtual bool SolveEERotation() const;
	virtual Interface_ID ExpectGoal() const;

	virtual const IKSys::ZeroPlaneMap* GetZeroPlaneMap(const Point3& , const Point3&) const { return NULL; }

	virtual float		GetPosThreshold() const;
	virtual float		GetRotThreshold() const;
	virtual unsigned	GetMaxIteration() const;
	virtual void		SetPosThreshold(float);
	virtual void		SetRotThreshold(float);
	virtual void		SetMaxIteration(unsigned);

	virtual ReturnCondition Solve(IKSys::LinkChain&);

	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return IK_SOLVER_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}
};


[!output TEMPLATESTRING_CLASSDESC]

[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


[!output CLASS_NAME]::[!output CLASS_NAME]()
{

}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{

}


bool [!output CLASS_NAME]::IsHistoryDependent() const
{
	#pragma message(TODO("Let the IK system know whether you a History dependent or not.  Return TRUE for History Dependent"))
	return false;
}

bool [!output CLASS_NAME]::DoesOneChainOnly() const
{
	#pragma message(TODO("Specify whether you can work on more than one chain or not"))
	return true; 
}


bool [!output CLASS_NAME]::IsInteractive() const
{
	#pragma message(TODO("Return whether you support Interactive ?"))
	return false; 
}

bool [!output CLASS_NAME]::UseSlidingJoint() const
{
	#pragma message(TODO("Do you use a sliding joint"))

	return false; 
}
	
bool [!output CLASS_NAME]::UseSwivelAngle() const
{
	#pragma message(TODO("DO you use the Swivel angle"))

	return true; 
}
  		
bool [!output CLASS_NAME]::IsAnalytic() const 
{
	#pragma message(TODO("Is this an Analytic solver - Return TRUE for yes"))
	return true; 
}


/******************************************************************************************************
*

	RotThreshold() is not relevant to solver that does not SolveEERotation().
	UseSwivelAngle() and SolveEERotation() cannot both be true.

*
\******************************************************************************************************/
bool [!output CLASS_NAME]::SolveEERotation() const {

	#pragma message(TODO("To be implemented"))
	return false; 
}

Interface_ID [!output CLASS_NAME]::ExpectGoal() const {
	#pragma message(TODO("To be implemented"))
	return kGoalID; 
}

float [!output CLASS_NAME]::GetPosThreshold() const
{
	#pragma message(TODO("Return the Position Threshold of the solver"))
	return 0.0f; 
}
	
float [!output CLASS_NAME]::GetRotThreshold() const
{

	#pragma message(TODO("Return the Rotation Threshold of the solver"))
	return 0.0f; 
}
		
unsigned [!output CLASS_NAME]::GetMaxIteration() const
{
	return 0; 
}

void [!output CLASS_NAME]::SetPosThreshold(float)
{
	#pragma message(TODO("Set the Position Threshold"))

}

void [!output CLASS_NAME]::SetRotThreshold(float)
{
	#pragma message(TODO("Set the Rotation Threshold"))
}

void [!output CLASS_NAME]::SetMaxIteration(unsigned)
{

}

/*************************************************************************************************
*
	This the method that is actually called to perform the IK specific task.
   
	This only use this method if you answers true to DoesOneChainOnly() and false to 
	HistoryDependent(). The solver is not designed to be invoked recursively. The
    recursion logic existing among the ik chains is taken care of by
    the Max IK (sub-)System.
*
\*************************************************************************************************/

IKSolver :: ReturnCondition [!output CLASS_NAME] :: Solve (IKSys::LinkChain& /*linkChain*/){

	#pragma message(TODO("Implement the actual IK Solver"))
	return 0;
}


