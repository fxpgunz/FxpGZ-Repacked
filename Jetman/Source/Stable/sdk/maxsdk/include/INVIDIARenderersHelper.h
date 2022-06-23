 /**********************************************************************
 *<
	DESCRIPTION: NVIDIA Renderers Helper

	CREATED BY:	David Lanier

 *>	Copyright (c) 2015 Autodesk, All Rights Reserved.
 **********************************************************************/

#pragma once

#include <ifnpub.h>

#define NVIDIA_RENDERERS_HELPER_INTERFACE_ID Interface_ID(0x52941f4d, 0x67936471)

/*! \sa  Class INVIDIARenderersHelper
\par Description:
This class provides an interface for working with NVIDIA Renderers. 
MAXScript and 3DS Max SDK developers can query whether mental ray dll is present or not and do things like grey out buttons in their UI if it is not present.

Usage in C++ Max SDK :
\code
INVIDIARenderersHelper* pINVIDIARenderersHelper = (INVIDIARenderersHelper*)GetCOREInterface(NVIDIA_RENDERERS_HELPER_INTERFACE_ID);
if (pINVIDIARenderersHelper){
    const bool bMentalrayDllLoaded = pINVIDIARenderersHelper->IsMentalrayDllPresent();
}
\endcode

Usage in Maxscript :
\code
bMentalrayRenderersLoaded   = NVIDIARenderersHelper.IsMentalrayDllPresent()
\endcode
*/

class INVIDIARenderersHelper : public FPStaticInterface
{
public:
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
	/** \brief Used to know if NVIDIA mental ray dll is present in 3DS Max.
	 * \return true if the dll is present
	 */
    virtual bool IsMentalrayDllPresent()const = 0;
};