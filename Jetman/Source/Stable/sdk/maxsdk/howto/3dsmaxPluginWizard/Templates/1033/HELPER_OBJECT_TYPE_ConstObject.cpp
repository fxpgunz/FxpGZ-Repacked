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

		// Parameter block
		IParamBlock2	*pblock;	//ref 0

		// Loading/Saving
		IOResult Load(ILoad *iload) {return IO_OK;}
		IOResult Save(ISave *isave) {return IO_OK;}

		//From Animatable
		Class_ID ClassID() {return [!output CLASS_NAME]_CLASS_ID;}		
		SClass_ID SuperClassID() { return HELPER_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, 
			PartID& partID, RefMessage message, BOOL propagate);

		int NumSubs() { return 1; }

		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

		void DeleteThis() { delete this; }		
		
		//Constructor/Destructor
		[!output CLASS_NAME]() {}
		~[!output CLASS_NAME]() {}

};


[!output TEMPLATESTRING_CLASSDESC]


[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]



