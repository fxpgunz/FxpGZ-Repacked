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

#include <NotificationAPI/NotificationAPI_Subscription.h>
#include <NotificationAPI/NotificationAPI_Events.h>
#include <RenderingAPI/Renderer/ICameraContainer.h>
#include <RenderingAPI/Translator/Helpers/INodeInstancingPool.h>

#pragma warning(push)
#pragma warning(disable: 4275 4251)

namespace MaxSDK
{;
namespace RenderingAPI
{;

using namespace NotificationAPI;

//==================================================================================================
// class BaseTranslator_ReferenceTarget
//
/*! Base class for translating an arbitrary ReferenceTarget.
    This base class provides basic, common functionality related to translating a ReferenceTarget. It handles:
    \li Monitoring the ReferenceTarget and invalidating the translator on any changes.
    \li Calling RenderBegin() on the ReferenceTarget before translation.
*/
class RenderingAPIExport BaseTranslator_ReferenceTarget : 
    public virtual Translator,
    // Watches for change notifications from the reference target
    protected INotificationCallback
{
public:

    /*! \param reference_target The ReferenceTarget to be translated.
        \param monitor_reference_target Whether the reference target should be directly monitored by this class. Should be set to false if the
            subclass handles the monitoring duties.
        \param graphNode The graph node to be passed to Translator::Translator(). */
    BaseTranslator_ReferenceTarget(
        ReferenceTarget* reference_target, 
        const bool monitor_reference_target,
        TranslatorGraphNode& graphNode);
    ~BaseTranslator_ReferenceTarget();

    // -- inherited from Translator
    virtual Interval CheckValidity(const TimeValue t, const Interval& previous_validity) const override;
    virtual void PreTranslate(const TimeValue translationTime, Interval& validity) override;
    virtual void PostTranslate(const TimeValue translationTime, Interval& validity) override;

protected:

    //! Returns the ReferenceTarget being handled by this class.
    ReferenceTarget* GetReferenceTarget() const;

    // -- inherited INotificationCallback
    virtual void NotificationCallback_NotifyEvent(const IGenericEvent& genericEvent, void* userData) override; 

private:

    const bool m_monitor_as_referencetarget;
    ReferenceTarget* const m_reference_target;
};

}}	// namespace 

#pragma warning(pop)
