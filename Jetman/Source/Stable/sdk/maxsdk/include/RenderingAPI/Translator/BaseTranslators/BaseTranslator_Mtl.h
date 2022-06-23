//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "BaseTranslator_MtlBase.h"

#pragma warning(push)
#pragma warning(disable: 4275 4251)

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class BaseTranslator_Mtl
//
/*! Base class for translating a material.
    This base class provides basic, common functionality related to translating a material. It handles:
    \li Monitoring the material and invalidating the translator on any changes.
    \li Calling RenderBegin() on the material before translation.
    \li Calling MtlBase::Update() on the material before translation.
*/
class RenderingAPIExport BaseTranslator_Mtl : 
    public BaseTranslator_MtlBase
{
public:

    //! \param mtl The material to be translated.
    //! \param graphNode The graph node to be passed to Translator::Translator().
    BaseTranslator_Mtl(Mtl* mtl, TranslatorGraphNode& graphNode);
    ~BaseTranslator_Mtl();

protected:

    //! Returns the material being handled by this translator.
    Mtl* GetMaterial() const;

    // -- inherited INotificationCallback
    virtual void NotificationCallback_NotifyEvent(const IGenericEvent& genericEvent, void* userData) override; 

private:

    Mtl* const m_mtl;
};

}}	// namespace 

#pragma warning(pop)
