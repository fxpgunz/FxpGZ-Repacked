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
#include "tab.h"
#include "maxtypes.h"
#include "AnimPropertyID.h"

// forward declarations
class AppDataChunk;
class ILoad;
class ISave;

// This list is maintained by the systems. Plug-ins need not concern themselves with it.
class AnimAppData : public AnimProperty {
	public:				
		Tab<AppDataChunk*> chunks;
		CRITICAL_SECTION csect;
		AppDataChunk *lastSearch;

		DWORD ID() {return PROPID_APPDATA;}		
		CoreExport ~AnimAppData();
		CoreExport AnimAppData();

		CoreExport AppDataChunk *FindChunk(const Class_ID& cid, SClass_ID sid, DWORD sbid);
		void AddChunk(AppDataChunk *newChunk) {chunks.Append(1,&newChunk);}
		CoreExport BOOL RemoveChunk(const Class_ID& cid, SClass_ID sid, DWORD sbid);

		CoreExport IOResult Load(ILoad *iload);
		CoreExport IOResult Save(ISave *isave);
	};