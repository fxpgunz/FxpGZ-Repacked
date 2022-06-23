//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////

/*! \file
 * \warning This API is unsupported and provided for experiment only.  It should not be used for production.
 * This API will change or disappear in a future release.
 */

#pragma once

#include "../dllutilities.h"

namespace MaxSDK
{;

inline QString QStringFromID(int resourceID)
{
    return QString(MaxSDK::GetResourceStringAsMSTR(resourceID));
}

}     // namespace
