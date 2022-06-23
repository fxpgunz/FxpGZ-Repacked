//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
#pragma once

#include "MaxHeap.h"
// forward declarations
namespace MaxSDK
{
	namespace AssetManagement
	{
		class AssetUser;
	}
}

// A callback object passed to EnumAuxFiles().
/*! \sa  Class Animatable, Class NameTab\n\n
\par Description:
This class is the callback object passed to <b>Interface::EnumAuxFiles()</b>
and to <b>Animatable::EnumAuxFiles()</b>. A developer derives a class from this
class and implements the <b>RecordAsset()</b> method to store each asset as it's
passed to it. At the end of the <b>EnumAuxFiles()</b> processing, the table of
names may be used. See Class NameTab for help
storing the names.  */
class AssetEnumCallback : public MaxHeapOperators
{
public:
	/*! \remarks Destructor. */
	virtual ~AssetEnumCallback() {}
	/*! \remarks This method is used to record the asset passed.
	\par Parameters:
	<b>const MaxSDK::AssetManagement::AssetUser& asset</b>\n\n
	The asset to store. */
	virtual void RecordAsset(const MaxSDK::AssetManagement::AssetUser& asset)=0;
};