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


#define PBLOCK_REF	0

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME] {
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	~[!output CLASS_NAME]();


	virtual const TCHAR* GetDefaultComment();
	// This samples a sequence for the area
	// This is the function that is called to get the next sample 
	virtual void DoSamples( ShadeOutput* pOut, SamplingCallback* cb, ShadeContext* sc, MASK pMask = NULL );
	// returns FALSE when out of samples
	virtual BOOL NextSample( Point2* pOutPt, float* pSampleSz, int n );
	// Integer number of samples for current quality setting
	virtual int GetNSamples();	

	// Get/Set quality and enable status
	virtual int   SupportsQualityLevels();
	virtual void  SetQuality( float q );
	virtual float GetQuality();

	virtual void SetEnable( BOOL on );
	virtual BOOL GetEnable();

	virtual ULONG SupportsStdParams() { return R3_ADAPTIVE; }

	virtual void SetAdaptiveOn( BOOL on );
	virtual BOOL IsAdaptiveOn();

	virtual void SetAdaptiveThreshold( float val );
	virtual float GetAdaptiveThreshold();

	// Loading/Saving
	virtual IOResult Load(ILoad *iload);
	virtual IOResult Save(ISave *isave);

	//From Animatable
	virtual Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}
	virtual SClass_ID SuperClassID() { return SAMPLER_CLASS_ID; }
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	virtual RefTargetHandle Clone( RemapDir &remap );
	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);

	virtual int NumSubs() { return 1; }
	virtual TSTR SubAnimName(int /*i*/) { return GetString(IDS_PARAMS); }
	virtual Animatable* SubAnim(int /*i*/) { return pblock; }

	// TODO: Maintain the number or references here
	virtual int NumRefs() { return 1; }
	virtual RefTargetHandle GetReference(int i);

	virtual int NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	virtual IParamBlock2* GetParamBlock(int /*i*/) { return pblock; } // return i'th ParamBlock
	virtual IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	void DeleteThis() { delete this; }

protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	// Parameter block
	IParamBlock2	*pblock;	//ref 0
};


[!output TEMPLATESTRING_CLASSDESC]

[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]



/*===========================================================================*\
 |	Sampler Implimentation
\*===========================================================================*/


[!output CLASS_NAME]::[!output CLASS_NAME]()
	: pblock(nullptr)
{
	Get[!output CLASS_NAME]Desc()->MakeAutoParamBlocks(this);
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{

}

RefTargetHandle [!output CLASS_NAME]::Clone( RemapDir &remap )
{
	[!output CLASS_NAME]* sksNew = new [!output CLASS_NAME]();
	sksNew->ReplaceReference(0,remap.CloneRef(pblock));
	BaseClone(this, sksNew, remap);
	return (RefTargetHandle)sksNew;
}


IOResult [!output CLASS_NAME]::Load(ILoad* iload)
{
	Sampler::Load(iload);
	return IO_OK;
}


IOResult [!output CLASS_NAME]::Save(ISave* isave)
{
	Sampler::Save(isave);
	return IO_OK;
}

const TCHAR* [!output CLASS_NAME]::GetDefaultComment()
{
	return GetString(IDS_COMMENT);
}

RefTargetHandle [!output CLASS_NAME]::GetReference(int i)
{
	if (i == PBLOCK_REF)
	{
		return pblock;
	}
	return nullptr;
}

void [!output CLASS_NAME]::SetReference(int i, RefTargetHandle rtarg )
{
	if (i == PBLOCK_REF)
	{
		pblock=(IParamBlock2*)rtarg;
	}
}

RefResult [!output CLASS_NAME]::NotifyRefChanged(
		const Interval& /*changeInterval*/, RefTargetHandle hTarget,
		PartID& /*partID*/,  RefMessage message, BOOL /*propagate*/) 
{
	switch (message)
	{
	case REFMSG_TARGET_DELETED:
		{
			if (hTarget == pblock)
			{
				pblock = nullptr;
			}
		}
		break;
	}
	return REF_SUCCEED;
}

void [!output CLASS_NAME]::DoSamples(ShadeOutput* /*pOut*/, SamplingCallback* /*cb*/, ShadeContext* /*sc*/, MASK /*pMask*/)
{
// TODO: Perform the actual sampling
}

BOOL [!output CLASS_NAME]::NextSample( Point2* /*pOut*/, float* /*pSampleSize*/, int /*n*/ )
{
	//TODO: Find next sample point
	return TRUE;
}


int [!output CLASS_NAME]::GetNSamples()
{
	// TODO: Return number of samples
	return 1;
}

/*===========================================================================*\
 |	Quality and Enabling support
\*===========================================================================*/

void [!output CLASS_NAME]::SetQuality( float q )
{ 
	pblock->SetValue( pb_quality, 0, q );
}

float [!output CLASS_NAME]::GetQuality()
{ 
	return pblock->GetFloat( pb_quality, 0);
}

int [!output CLASS_NAME]::SupportsQualityLevels() 
{
	return 1; 
}

void [!output CLASS_NAME]::SetEnable( BOOL on )
{ 
	pblock->SetValue( pb_enable, 0, on );
}

BOOL [!output CLASS_NAME]::GetEnable()
{ 
	BOOL b; 
	Interval valid;
	pblock->GetValue( pb_enable, 0, b, valid );
	return b;
}

/*===========================================================================*\
 |	Adaptive Sampling support
\*===========================================================================*/


void [!output CLASS_NAME]::SetAdaptiveOn( BOOL on )
{ 
	pblock->SetValue( pb_adapt_enable, 0, on ); 
}

BOOL [!output CLASS_NAME]::IsAdaptiveOn()
{ 
	BOOL b; 
	Interval valid;
	pblock->GetValue( pb_adapt_enable, 0, b, valid );
	return b;
}

void [!output CLASS_NAME]::SetAdaptiveThreshold( float val )
{ 
	pblock->SetValue( pb_adapt_threshold, 0, val ); 
}

float [!output CLASS_NAME]::GetAdaptiveThreshold() 
{ 
	return pblock->GetFloat( pb_adapt_threshold, 0);
}
