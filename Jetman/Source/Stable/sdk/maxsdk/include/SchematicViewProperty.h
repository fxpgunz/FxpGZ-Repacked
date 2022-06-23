//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
#pragma once

#include "CoreExport.h"
#include "AnimProperty.h"
#include "AnimPropertyID.h"
#include "maxtypes.h"

// forward declarations
class ILoad;
class ISave;

static const DWORD SV_NO_REF_INDEX = 0xFFFFFFFF;
class SchematicViewProperty : public AnimProperty
{
private:
	DWORD nodeRefIndex;

public:				
	DWORD ID() { return PROPID_SVDATA; }
	CoreExport ~SchematicViewProperty() {}
	CoreExport SchematicViewProperty();

	DWORD GetRefIndex();
	void SetRefIndex(DWORD refIndex);
	bool GetRefSaveMark();

	CoreExport IOResult Load(ILoad *iload);
	CoreExport IOResult Save(ISave *isave);
};