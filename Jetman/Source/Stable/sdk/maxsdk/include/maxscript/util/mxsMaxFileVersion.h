//**************************************************************************/
// Copyright (c) 1998-2009 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/

#pragma once

#include "../ScripterExport.h"
#include "../../plugapi.h"

/** 
 * \brief The 3ds Max file versions the maxscript save commands 
 * (saveMaxFile and saveNodes) work with.
 * These values are converted by GetMaxVerFromMxsMaxFileVer() to 
 * the ones in \ref VersionMacros such as MAX_RELEASE, etc
*/
enum EMxsMaxFileVersion 
{
	//! \brief Unsupported 3ds Max version, corresponds to MAX_RELEASE_UNSUPPORTED
	kMxsMaxFileVersion_Unsuported = -1,
	//! \brief A file version was not specified, 
	//! maxscript save commands default to saving to current file version (MAX_RELASE)
	kMxsMaxFileVersion_Unspecified = 0,
	//! \brief The current 3ds Max version, corresponds to MAX_RELEASE
	kMxsMaxFileVersion_Current = 1,

	//! \brief 3ds Max 2015 file version, corresponds to MAX_RELEASE_R17
	kMxsMaxFileVersion_2015 = 2015,
	//! \brief 3ds Max 2016 file version, corresponds to MAX_RELEASE_R18
	kMxsMaxFileVersion_2016 = 2016,
	//! \brief 3ds Max 2017 file version, corresponds to MAX_RELEASE_R19
	kMxsMaxFileVersion_2017 = 2017,
	//! \brief 3ds Max 2018 file version, corresponds to MAX_RELEASE_R20
	kMxsMaxFileVersion_2018 = 2018,
};

/** 
 * \brief Converts a "human readable" 3ds Max version to \ref VersionMacros.
 * \param mxsMaxFileVer 3ds Max version as it occurs in the product name
 * \return A value in \ref VersionMacros (such as MAX_RELEASE),
 * or MAX_RELEASE_UNSUPPORTED if a non-supported 3ds Max version is specified.
*/
ScripterExport long GetMaxVerFromMxsMaxFileVer(EMxsMaxFileVersion mxsMaxFileVer);

/** 
 * \brief Converts a given 3ds Max version number to a EMxsMaxFileVersion.
 * \param maxVersion 3ds Max version number
 * \return The version number as it occurs in the product name ,
 * or kMxsMaxFileVersion_Unsuported if a non-supported 3ds Max version is specified.
*/
ScripterExport EMxsMaxFileVersion GetMxsMaxFileVerFromMaxVer(long maxVersion);
