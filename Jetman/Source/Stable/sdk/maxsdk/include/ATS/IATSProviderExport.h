//**************************************************************************/
// Copyright (c) 1998-2005 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: DLL Export functions for ATSProvider DLLs 
// AUTHOR: Michael Russo - created January 25, 2005
//***************************************************************************/

#pragma once

#include "IATSProvider.h"

#ifndef ATSProviderExport
	#define ATSProviderExport __declspec( dllexport )
#endif

#ifndef ATSExternC
	#define ATSExternC extern "C"
#endif

//! \brief ATS Version Number
/*! \see IATSProvider
\return ATS Version number: kATSAPIVersion
*/
ATSExternC ATSProviderExport DWORD			GetATSVersion( void );

//! \brief ATS Provider name
/*! \see IATSProvider
\return string contain the provider's name
*/
ATSExternC ATSProviderExport const MCHAR*	GetATSProviderName( void );

//! \brief IATSProvider pointer
/*! \see IATSProvider
\return pointer to IATSProvider interface
*/
ATSExternC ATSProviderExport IATSProvider*	GetIATSProvider( void );

//! \brief Release IATSProvider pointer
/*! \see IATSProvider
\param[in] pIATSProvider point to IATSProvider interface returned from
GetIATSProvider that needs to be released.
*/
ATSExternC ATSProviderExport void			ReleaseIATSProvider( IATSProvider *pIATSProvider );

