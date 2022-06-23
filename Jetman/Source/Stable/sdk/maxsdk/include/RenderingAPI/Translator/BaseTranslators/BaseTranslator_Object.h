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

// Max SDK
#include <NotificationAPI/NotificationAPI_Subscription.h>
#include <NotificationAPI/NotificationAPI_Events.h>
#include <RenderingAPI/Renderer/ICameraContainer.h>
#include <RenderingAPI/Translator/Helpers/INodeInstancingPool.h>

// std
#include <memory>

#pragma warning(push)
#pragma warning(disable: 4275 4251)

namespace MaxSDK
{;
namespace RenderingAPI
{;
using namespace NotificationAPI;

//==================================================================================================
// class BaseTranslator_Object
//
/*! Base class for translating the object referenced by a node instancing pool.
    This base class provides basic, common functionality related to translating the object referenced by an INode. It handles:
    \li Monitoring the node instancing pool and invalidating the translator on any changes.
    \li Calling RenderBegin() on the INode before translation.
*/
class RenderingAPIExport BaseTranslator_Object : 
    public virtual Translator,
    // Watches for change notifications from the node pool
    protected TranslationHelpers::INodeInstancingPool::INotifier,
    // Watches for change notifications from the material
    private INotificationCallback
    {
public:

    //! \param nodePool The node instancing pool from which the object is to be evaluated.
    //! \param graphNode The graph node to be passed to Translator::Translator().
    BaseTranslator_Object(const std::shared_ptr<const TranslationHelpers::INodeInstancingPool>& nodePool, TranslatorGraphNode& graphNode);
    ~BaseTranslator_Object();

    // -- inherited from Translator
    virtual Interval CheckValidity(const TimeValue t, const Interval& previous_validity) const override;
    virtual void PreTranslate(const TimeValue translationTime, Interval& validity) override;
    virtual void PostTranslate(const TimeValue translationTime, Interval& validity) override;

protected:

    /*! Returns the node from which the object is evaluated.
        This node is extracted from the node instancing pool, through INodeInstancingPool::GetRepresentativeNode(). */
    INode* GetNode() const;

    //! Returns the node instancing pool being handled by this translator.
    std::shared_ptr<const TranslationHelpers::INodeInstancingPool> GetNodePool() const;

    //! Returns whether this translator takes into account displacement properties from the material. That is, whether
    //! a change in the material's displacement properties should cause this translator to be invalidated.
    virtual bool CareAboutMaterialDisplacement() const = 0;

    // -- inherited from INodeInstancingPool::INotifier
    virtual void NotifyPoolInstancedObjectChanged(const TranslationHelpers::INodeInstancingPool& node_pool, const NodeEventType event_type) override;
    virtual void NotifyPoolContentMaybeChanged(const TranslationHelpers::INodeInstancingPool& node_pool) override;

private:

    // Delete the copy constructor, to solve a bogus compile error in VS2015 prior to Update 1:
    // basetranslator_object.h(90): error C2280: 'MaxSDK::RenderingAPI::Translator::Translator(const MaxSDK::RenderingAPI::Translator &)': attempting to reference a deleted function
    BaseTranslator_Object(const MaxSDK::RenderingAPI::BaseTranslator_Object&) = delete;

    // Internally used to change the node material being monitored for change
    void set_monitored_material(Mtl* const mtl);

    // -- inherited from INotificationCallback
    virtual void NotificationCallback_NotifyEvent(const IGenericEvent& genericEvent, void* userData) override final;

private:

    const std::shared_ptr<const TranslationHelpers::INodeInstancingPool> m_node_pool;

    // Saves whether the last translation took into account displacement from the material
    bool m_last_translation_has_material_displacement = false;
    // Material currently being monitored
    Mtl* m_monitored_node_material = nullptr;
};

}}	// namespace 

#pragma warning(pop)
