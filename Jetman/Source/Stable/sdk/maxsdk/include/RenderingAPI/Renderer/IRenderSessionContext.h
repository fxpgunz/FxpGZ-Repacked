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

#include "MotionBlurSettings.h"

#include <maxtypes.h>
#include <NotificationAPI/InteractiveRenderingAPI_Subscription.h>
#include <RenderingAPI/Translator/Helpers/INodeInstancingPool.h>

#include <vector>

class Box2;
class ReferenceMaker;
class IRenderElementMgr;
class DefaultLight;
class IRenderElement;
class Atmospheric;
class Effect;

namespace MaxSDK
{;
namespace RenderingAPI
{;
class ISceneContainer;
class ICameraContainer;
class IRenderSettingsContainer;
class IEnvironmentContainer;
class IRenderingProcess;
class IRenderingLogger;
class IFrameBufferProcessor;
class IRenderSession;
class ITranslationManager;

//==================================================================================================
// class IRenderSessionContext
//
/*! Encapsulates all that a render session needs to render a frame.
    
    This interface encapsulates the definition of the scene which is to be rendered by a render session. It also exposes functionality
    related to evaluating the scene and processing the frame buffer.
*/
class IRenderSessionContext
{
public:

    class IChangeNotifier;

    //! \name Scene Definition
    //@{
    //! Returns the scene container, which wraps the set of objects to be rendered.
    virtual const ISceneContainer& GetScene() const = 0;
    //! Returns the camera container, which abstracts the attributes of the camera to be rendered.
    virtual const ICameraContainer& GetCamera() const = 0;
    //! Returns the environment object, which encapsulates the definition of background and environment maps and colors.
    virtual IEnvironmentContainer* GetEnvironment() const = 0;
    //! Returns the render settings container, which encapsulates the options that affect the translation and rendering processes.
    virtual const IRenderSettingsContainer& GetRenderSettings() const = 0;
    //@}

    //! \name Default Lighting
    //! Default lighting is used when there are no other sources of light in the scene.
    //@{
    /*! Helper method for determining whether default lighting should be enabled.
        This method returns true if and only there are no sources of lighting in the scene (i.e. no light nodes). It may optionally consider
        the environment to be a source of illumination.
        \param considerEnvironmentLighting Set to true to consider a non-black environment to be a source of light.
		\param t The TimeValue at which to determine whether lighting should be enabled.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter.
        \return True if and only if default lights should be enabled. */
    virtual bool GetDefaultLightingEnabled(const bool considerEnvironmentLighting, const TimeValue t, Interval& validity) const = 0;
    /*! Returns the set of default lights to be used if there is no other source of lighting in the scene. 
        \remark It is up to the renderer integration to decide whether to use the default lights or not, as each renderer may have its own set of
                criteria. For example, some (but not all) renderers consider an environment map to be a source of lighting. */
    virtual std::vector<DefaultLight> GetDefaultLights() const = 0;
    //@}

    //! \name Process Management and Message Logging
    //@{
    //! Returns the interface used for reporting rendering/translation events and progress, as well as to executes jobs from the main thread.
    virtual IRenderingProcess& GetRenderingProcess() const = 0;
    //! Returns the interface used for logging messages.
    virtual IRenderingLogger& GetLogger() const = 0;
    //@}

    //! \name RenderBegin() / RenderEnd()
    //! Animatable::RenderBegin() must be called on scene objects before they are translated.
    //@{
    /*! Calls RenderBegin() on the given object and its entire reference sub-hierarchy.
        It's important to call this method on any Animatable before it's accessed for translation or rendering. The standard set of objects on which
        RenderBegin() should be called includes:
            \li scene nodes (geometric, light, helper)
            \li render elements
            \li atmospherics
            \li render effects
            \li environment

        \remark This method keeps track of all objects for which it has called RenderBegin(), and will never call it more than once for a given object. 
        \param refMaker The object on which to call RenderBegin().
        \param t The time at which the scene is being translated / rendered. If rendering multiple frames, this is the time of the <i>first</i> frame. */
    virtual void CallRenderBegin(ReferenceMaker& refMaker, const TimeValue t) = 0;
    /*! Calls RenderEnd() on every object for which CallRenderBegin() was called.
        This is called automatically by the system under the following conditions:
            \li For <b>offline renedering</b>, UnifiedRenderer calls this in Close().
            \li For <b>interactive rendering</b>, ITranslationManager calls this just after a scene translation / update.

        If needed - generally, if not using ITranslationManager for interactive rendering - the renderer may call this explicitly. 
        \param t The time at which the scene has been last translated / rendered. If rendering multiple frames, this is the time of the <i>last</i> frame. */
    virtual void CallRenderEnd(const TimeValue t) = 0;
    //@}

    //! \name Frame Buffer Processing
    //@{
    /*! Returns the interface used to store data into the main frame buffer.
        This interface provides functionality for efficiently transferring the renderer's own frame buffer into the 3ds Max frame buffer, optionally
        process the active ToneOperator.
        \remark This method <i>thread safe</i>; it may be called from any thread. Doing so enables the ToneOperator to be processed in that thread,
            which avoids blocking the main thread. Interactive renderers should thus call this from their separate update thread. 
        \remark The render Bitmap should not be modified or accessed in parallel to this call.*/
    virtual IFrameBufferProcessor& GetMainFrameBufferProcessor() = 0;
    /*! Refreshes the frame buffer window, after the frame buffer bitmap has been updated.
        This method abstracts the different mechanism for updating the bitmap of an offline or interactive renderer.
        \remark This method <i>thread safe</i>: it may be called from any thread. 
        \remark The render Bitmap should not be modified or accessed in parallel to this call. */
    virtual void UpdateBitmapDisplay() = 0;
    //@}


    //! \name Node Evaluation
    //! Provides methods to help evaluate node transforms, taking into account motion blur.
    //@{
    /*! Evaluates the transforms of the given node, taking motion blur into account.
        \remark To evaluate a node's transform without taking motion blur into account, pass custom motion blur settings that explicitly disable motion blur.
        \param node The node for which to evaluate the transforms.
        \param t The time at which to evaluate the node.
        \param [in, out] validity The validity of the evaluated transforms is intersected into this. The validity of both transforms is taken into account correctly.
        \param node_motion_blur_settings Optional, custom motion blur settings to be used when evaluating the node transforms. If null, the settings are
            obtained by calling GetMotionBlurSettingsForNode().
        \return The motion transforms, as evaluated for the provided node. */
    virtual MotionTransforms EvaluateMotionTransforms(
        INode& node, 
        const TimeValue t, 
        Interval& validity, 
        const MotionBlurSettings* node_motion_blur_settings = nullptr) const = 0;
    /*! Evaluates the motion blur settings associated with this node.
        This method takes the global motion blur settings as input, modifies them according to the node's object properties, and returns the result.
        If, for example, the node's object properties disable motion blur, then motion blur will be disabled in the node's settings.
        \param node The node for which the motion blur settings are to be evaluated.
        \param t The time at which to evaluate the motion blur settings.
        \param validity [in, out] The validity of the evaluated settings is intersected into this.
        \param global_motion_blur_settings Optional, global motion blur settings which are to be modified by the node's properties. If null, the global
            settings are obtained from ICameraContainer.
        \return The motion blur settings to be used for the given node.
        \see UnifiedRenderer::MotionBlurIgnoresNodeProperties() */
    virtual MotionBlurSettings GetMotionBlurSettingsForNode(
        INode& node, 
        const TimeValue t, 
        Interval& validity, 
        const MotionBlurSettings* global_motion_blur_settings = nullptr) const = 0;
    //@}

    //! \name Miscellaneous
    //@{
    /*! Returns the set of render elements to be used by this session.
        \remark This method only returns those elements that are enabled; the caller therefore does not need to check for IRenderElementMgr::GetElementsActive()
        or IRenderElement::IsEnabled(). 
        \remark The system calls RenderBegin() on all render elements before translation begins. */
    virtual std::vector<IRenderElement*> GetRenderElements() const = 0;
    /*! Returns the set of atmospherics to be used by this render session.
        \remark The system calls RenderBegin() on all atmospherics before translation begins. */
    virtual std::vector<Atmospheric*> GetAtmospherics() const = 0;
    /*! Returns the top-level render effect currently being used. In most cases this can be cast to a IRenderEffectsMgr, which wraps and manages the actual
        collection of render effects to be used.
        \remark The system calls RenderBegin() on all effects before translation begins. */
    virtual Effect* GetEffect() const = 0;
    //! Returns the render session to which this context is associated.
    virtual IRenderSession* GetRenderSession() const = 0;
    /*! Returns the notification client to be used for monitoring scene changes. 
        \remark Will return null if scene changes need not be monitored, for example when performing offline (non-interactive) rendering. */
    virtual NotificationAPI::IImmediateInteractiveRenderingClient* GetNotificationClient() const = 0;
    /*! Returns the node instancing manager.
        The node instancing manager is used for collecting those nodes which are all instances of the same geometric object. The complex logic, required
        to handle instancing correctly, is fully encapsulated by this interface. */
    virtual TranslationHelpers::INodeInstancingPool::IManager& GetNodeInstancingManager() const = 0;
    /*! Returns the translation manager.
        The translation manager is an optional component which may be used to manage the scene translation graph. */
    virtual ITranslationManager& GetTranslationManager() const = 0;
    /*! Sets the down-resolution factor.
        The renderer plugin may decide that it wishes to lower the resolution at which the scene is rendered, e.g. to improve
        the performance of interactive renders in an adaptive manner. This method will set the factor at which the resolution
        is lowered, ensuring that all related scene properties are correctly affected - including the resolution, render region,
        image plane offset, etc.
        \remark This method is thread safe and may thus be called from the interactive rendering thread.
        \remark This method will trigger change notifications as necessary. For example, the camera will broadcast notifications if its
            resolution changes.
        \param factor The factor by which the resolution is to be reduced. The internal default is 1, meaning that the resolution is not reduced.
            A value of 0 is not valid. A value greater than the resolution will be clamped.
        \return The full render resolution, unaffected by the down-res factor. */
    virtual IPoint2 SetDownResolutionFactor(const unsigned int factor) = 0;
    //@}

    //! \name Notification
    //@{
    //! Registers a notifier callback, to be called when interactive changes occur.
    virtual void RegisterChangeNotifier(IChangeNotifier& notifier) const = 0;
    //! Unregisters a notification callback, previously registered with RegisterChangeNotifier().
    virtual void UnregisterChangeNotifier(IChangeNotifier& notifier) const = 0;
    //@}

protected:

    //! Protected destructor prevents deletion through this interface.
    virtual ~IRenderSessionContext() {}
};

//==================================================================================================
// class IRenderSessionContext::IChangeNotifier
//
/*! Notification callback interface for IRenderSessionContext.

    When registered with an IRenderSessionContext, this interface will be called whenever a relevant change occurs.
*/
class IRenderSessionContext::IChangeNotifier
{
public:
    //! Notifies that the set of default lights has changed, or that the return value of GetDefaultLightingEnabled() may have changed.
    virtual void NotifyDefaultLightingMaybeChanged() = 0;
protected:
    //! Protected destructor prevents deletion through this interface
    virtual ~IChangeNotifier() {}
};


}}	// namespace 