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

static const Class_ID [!output CLASS_NAME]_CLASS_ID([!output CLASSID1], [!output CLASSID2]);

#define PBLOCK_REF	0


class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME]
{
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	virtual ~[!output CLASS_NAME]();

	virtual TSTR GetName();

	// Kernel parameter setup and handling
	virtual long GetNFilterParams();
	virtual const TCHAR * GetFilterParamName( long nParam );
	virtual double GetFilterParam( long nParam );
	virtual void SetFilterParam( long nParam, double val );
	virtual const TCHAR * GetDefaultComment();

	virtual void Update(TimeValue t, Interval& valid);

	// Actual kernel function
	virtual double KernelFn( double x, double y );

	// Kernel functionality queries
	virtual long GetKernelSupport();
	virtual long GetKernelSupportY();
	virtual bool Is2DKernel();
	virtual bool IsVariableSz();
	virtual void SetKernelSz( double x, double y = 0.0 );
	virtual void GetKernelSz( double& x, double& y );
	virtual bool IsNormalized();
	virtual bool HasNegativeLobes();

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return FILTER_KERNEL_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir &remap );
	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget,
		PartID& partID, RefMessage message, BOOL propagate);


	virtual int NumSubs() { return 1; }
	virtual TSTR SubAnimName(int /*i*/) { return GetString(IDS_PARAMS); }
	virtual Animatable* SubAnim(int /*i*/) { return pblock; }

	// TODO: Maintain the number or references here
	virtual int NumRefs() { return 1; }
	virtual RefTargetHandle GetReference(int i);

	virtual int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	virtual void DeleteThis();

protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	// We store 2 parameters
	double param1, param2;

	// Parameter block
	IParamBlock2	*pblock;	//ref 0
};


[!output TEMPLATESTRING_CLASSDESC]


[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]



[!output CLASS_NAME]::[!output CLASS_NAME]()
:	pblock(nullptr)
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{

}

/*===========================================================================*\
 |	Kernel Parameter returns
\*===========================================================================*/

// Number of parameters we support
long [!output CLASS_NAME]::GetNFilterParams() { return 2; }

// Get the name of parameter #nParam
const TCHAR * [!output CLASS_NAME]::GetFilterParamName( long nParam )
{
	return GetString( nParam ? IDS_PARAM2:IDS_PARAM1 );
}

// Get the value of parameter #nParam
double [!output CLASS_NAME]::GetFilterParam( long nParam )
{
	return nParam ? param2 : param1;
}

// Set our parameter variables
void [!output CLASS_NAME]::SetFilterParam( long nParam, double val )
{
	if (nParam) {
		param2 = val;
		pblock->SetValue( pb_param2, 0, float( val ) );
 	} else {
		param1 = val;
		pblock->SetValue( pb_param1, 0, float( val ) );
	}
}

void [!output CLASS_NAME]::Update(TimeValue t, Interval& valid){
	float val;
	pblock->GetValue( pb_param1, t, val, valid ); param1 = val;
	pblock->GetValue( pb_param2, t, val, valid ); param2 = val;
}

const TCHAR * [!output CLASS_NAME]::GetDefaultComment()
{
	return GetString( IDS_COMMENT);
}

IOResult [!output CLASS_NAME]::Load(ILoad *iload)
{
	FilterKernel::Load(iload);
	return IO_OK;
}
IOResult [!output CLASS_NAME]::Save(ISave *isave)
{
	FilterKernel::Save(isave);
	return IO_OK;
}


RefResult [!output CLASS_NAME]::NotifyRefChanged(
		const Interval& /*changeInt*/, RefTargetHandle hTarget,
		PartID& /*partID*/,  RefMessage message, BOOL /*propagate*/) 
{
	switch (message) {
		case REFMSG_CHANGE:
			[!output CLASS_NAME_LOWER]_param_blk.InvalidateUI();
			break;
		case REFMSG_TARGET_DELETED:
			if (hTarget == pblock)
			{
				pblock = nullptr;
			}
			break;
		}
	return REF_SUCCEED;
}

RefTargetHandle [!output CLASS_NAME]::Clone( RemapDir &remap )
{
	[!output CLASS_NAME]* newObj = new [!output CLASS_NAME]();
	newObj->ReplaceReference(0,remap.CloneRef(pblock));
	BaseClone(this, newObj, remap);
	return (RefTargetHandle)newObj;
}



/*===========================================================================*\
 |	Calculate a result
\*===========================================================================*/

double [!output CLASS_NAME]::KernelFn( double x, double )
{
	if ( x < param1 ) return 1.0;
	if ( x > param2 ) return 1.0;
	else return 0.0;
}


/*===========================================================================*\
 |	Kernel functionality queries
\*===========================================================================*/

// Integer number of pixels from center to filter 0 edge.
// Must not truncate filter x dimension for 2D filters
long [!output CLASS_NAME]::GetKernelSupport(){ return 1; }

// For 2d returns y support, for 1d returns 0
long [!output CLASS_NAME]::GetKernelSupportY(){ return 0; }

// Are we 2D or Variable Size?
bool [!output CLASS_NAME]::Is2DKernel(){ return FALSE; }
bool [!output CLASS_NAME]::IsVariableSz(){ return FALSE; }

// 1-D filters ignore the y parameter, return it as 0.0
void [!output CLASS_NAME]::SetKernelSz( double /*x*/, double /*y*/){}
void [!output CLASS_NAME]::GetKernelSz( double& x, double& y ){ x = 0.5; y = 0.0; }

// Returning true will disable the built-in normalizer
bool [!output CLASS_NAME]::IsNormalized(){ return FALSE; }

// This is for possible future optimizations
bool [!output CLASS_NAME]::HasNegativeLobes(){ return FALSE; }

void [!output CLASS_NAME]::SetReference( int i, RefTargetHandle rtarg )
{
	if (i == PBLOCK_REF)
	{
		pblock = (IParamBlock2*)rtarg;
	}
}

RefTargetHandle [!output CLASS_NAME]::GetReference( int i)
{
	if (i == PBLOCK_REF)
	{
		return pblock;
	}
	return nullptr;
}

void [!output CLASS_NAME]::DeleteThis()
{
	delete this;
}

TSTR [!output CLASS_NAME]::GetName()
{
	return GetString(IDS_CLASS_NAME);
}