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

#include "../Translator.h"
#include "../../Renderer/ICameraContainer.h"

#pragma warning(push)
#pragma warning(disable: 4275 4251)

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class BaseTranslator_Camera
//
/*! Base class for translating the scene camera.
    This base class provides basic, common functionality related to translating the scene camera. It handles:
    \li Monitoring the scene camera and invalidating the translator on any changes.
    \li Calling RenderBegin() on the camera node before translation.
*/
class RenderingAPIExport BaseTranslator_Camera : 
    public virtual Translator,
    // Watches for change notifications from the camera
    protected ICameraContainer::IChangeNotifier
{
public:

    //! This contructor takes the same arguments as Translator::Translator().
    BaseTranslator_Camera(TranslatorGraphNode& graphNode);
    ~BaseTranslator_Camera();

    // -- inherited from Translator
    virtual Interval CheckValidity(const TimeValue t, const Interval& previous_validity) const override;
    virtual void PreTranslate(const TimeValue translationTime, Interval& validity) override;
    virtual void PostTranslate(const TimeValue translationTime, Interval& validity) override;
    virtual MSTR GetTimingCategory() const override;

protected:

    // -- inherited from CameraContainer::IChangeNotifier
    // Simply invalidates the translator
    virtual void NotifyCameraChanged() override;

};

}}	// namespace 

#pragma warning(pop)
