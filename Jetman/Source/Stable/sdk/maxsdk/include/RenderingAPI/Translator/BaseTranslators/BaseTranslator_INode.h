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

#include <vector>

#pragma warning(push)
#pragma warning(disable: 4275 4251)

namespace MaxSDK
{;
namespace RenderingAPI
{;
using namespace NotificationAPI;

//==================================================================================================
// class BaseTranslator_INode
//
/*! Base class for translating an INode.
    This base class provides basic, common functionality related to translating an INode. It handles:
    \li Monitoring the INode and invalidating the translator on any changes.
    \li Calling RenderBegin() on the INode before translation.
    \li Evaluation of the node transform matrices and properties.
*/
class RenderingAPIExport BaseTranslator_INode : 
    public virtual Translator,
    // Watches for change notifications from the node
    protected INotificationCallback,
    // Watches for change notifications from the camera, for motion blur options
    protected ICameraContainer::IChangeNotifier,
    // Watches when the node gets ejected from the pool
    protected TranslationHelpers::INodeInstancingPool::INotifier
{
public:

    //! \param node The node to be translated.
    //! \param notifierType The node's notifier type, to be used with the Notification System.
    //! \param graphNode The graph node to be passed to Translator::Translator().
    BaseTranslator_INode(
        INode& node, 
        const NotifierType notifierType, 
        TranslatorGraphNode& graphNode);
    ~BaseTranslator_INode();

    // -- inherited from Translator
    virtual Interval CheckValidity(const TimeValue t, const Interval& previous_validity) const override;
    virtual void PreTranslate(const TimeValue translationTime, Interval& validity) override;
    virtual void PostTranslate(const TimeValue translationTime, Interval& validity) override;

protected:

    /*! Evaluates and returns the transform matrices for this INode.
        The evaluation takes into account the current motion blur parameters, from the INode and from the scene.
        The two transforms will be identical if and only motion blur is disabled for the INode.
        \param t The time at which to evaluate the node transforms.
        \param [in, out] validity The validity interval of the node transforms will be intersected into this one. */
    MotionTransforms EvaluateTransforms(const TimeValue t, Interval& validity) const;

    //! Returns the node being handled by this translator.
    INode& GetNode() const;

    //! Returns the material assigned to the node, resolving any wrapper materials in the process (e.g. Shell material, scripted material).
    Mtl* GetMaterial() const;

    /*! Returns the node pool to which this node belongs, useful for translating the node's Object with support for instances.
        A single Object need be translated for every node pool. The node pool may change between translations and therefore must be queried every time.
        \param t The time used when evaluating the node to construct the instancing pool.
        \see INodeInstancingPool::IManagerGetPoolForNode() */
    std::shared_ptr<const TranslationHelpers::INodeInstancingPool> GetNodePool(const TimeValue t);

    /*! Enables the implementation to override what notification messages it wants to invalidate this translator.
        \param eventType The event type to potentially filter.
        \return True if the event needs to evaluate this translator, false otherwise - in which case the event is ignored. */
    virtual bool CareAboutNotificationEvent(const NodeEventType eventType) const;
    /*! Enables the implementation to specify to determine whether it cares about a specific node property changing.
        This method covers the properties \ref partid_node_render_property "PART_REND_PROP_*". */
    virtual bool CareAboutRenderProperty(const PartID render_property_id) const;
    /*! Enables the implementation to specify to determine whether it cares about a specific node property changing.
        This method covers the properties \ref partid_disp_props "PART_DISP_PROP_*". */
    virtual bool CareAboutDisplayProperty(const PartID display_property_id) const;
    /*! Enables the implementation to specify to determine whether it cares about a specific node property changing.
        This method covers the properties \ref partid_node_global_illumination_property "PART_GI_*". */
    virtual bool CareAboutGIProperty(const PartID gi_property_id) const;

    /*! Resolves the given material by discarding any "wrapper" materials that should be ignored by the renderer. 
        For example, in the case of a Shell material, this returns the Shell's sub-material that is flagged for rendering.
        This method may be overridden if necessary, to add support for renderer-specific behaviour. 
        \param mtl The material to be resolved (or un-wrapped).
        \return The resolved material, free of wrappers which the renderer may not understand directly. */
    virtual Mtl* ResolveMaterial(Mtl* const mtl) const;

    /*! Returns whether this translator translates a mesh for which we need to care if UVW channels are required by the material but missing on the mesh.
        These missing UVW channels will be automatically reported by the system. 
        \sa GetMeshUVWChannelIDs()
        \sa GetMeshMaterialIDs() */
    virtual bool CareAboutMissingUVWChannels() const = 0;
    /*! Returns the UVW channel IDs which are used by the mesh that this node references.
        This is used to determine whether the node's material is dependent on any missing channels, for error reporting purposes.
        This is called in PostTranslate(), so it's recommended tha the list of UVW channels IDs be cached during the translation of the
        object.
        \remark This is only called if CareAboutMissingUVWChannels() returns true. */
    virtual std::vector<unsigned int> GetMeshUVWChannelIDs() const = 0;
    /*! Returns the set of material IDs referenced by this mesh.
        This is used to determine whether the node's material is dependent on any missing channels, for error reporting purposes.
        \remark This is only called if CareAboutMissingUVWChannels() returns true. */
    virtual std::vector<MtlID> GetMeshMaterialIDs() const = 0;

    // -- inherited from INotificationCallback
    virtual void NotificationCallback_NotifyEvent(const IGenericEvent& genericEvent, void* userData) override final;

    // -- inherited from ICameraContainer::IChangeNotifier
    virtual void NotifyCameraChanged() final;

    // -- inherited from INodeInstancingPool::INotifier
    virtual void NotifyPoolInstancedObjectChanged(const TranslationHelpers::INodeInstancingPool& node_pool, const NodeEventType event_type) override;
    virtual void NotifyPoolContentMaybeChanged(const TranslationHelpers::INodeInstancingPool& node_pool) override;

private:

    // Delete the copy constructor, to solve a bogus compile error in VS2015 prior to Update 1:
    // basetranslator_inode.h(161): error C2280: 'MaxSDK::RenderingAPI::Translator::Translator(const MaxSDK::RenderingAPI::Translator &)': attempting to reference a deleted function
    BaseTranslator_INode(const MaxSDK::RenderingAPI::BaseTranslator_INode&) = delete;

    // Internally used to change the node pool which is being monitored
    void set_node_pool(const std::shared_ptr<const TranslationHelpers::INodeInstancingPool>& new_pool);
    // Internally used to change the node material being monitored for change
    void set_monitored_material(Mtl* const mtl);

    // Check the given material for missing UVW channels, reporting the missing channels as necessary.
    // Doesn't support multi/sub-object materials... assumes the sub-materials are passed here
    void CheckMaterialForMissingUVWChannels(Mtl& mtl, const std::vector<unsigned int>& channels_present);

private:

    INode& m_node;

    // The node pool currently in use, currently being monitored
    std::shared_ptr<const TranslationHelpers::INodeInstancingPool> m_node_pool;
    // Flags whether the node pool is invalid and needs to be re-acquired.
    bool m_node_pool_invalid;

    // Stores the motion blur properties used during the last translation. Used to determine when they change.
    mutable MotionBlurSettings m_last_translated_motion_settings;

    const NotifierType m_notifierType;

    // Cached, resolved material assigned to the node
    Mtl* m_resolved_material;
    // Node material, monitored to know when the resolved material might change
    Mtl* m_monitored_node_material;
};

}}	// namespace 

#pragma warning(pop)
