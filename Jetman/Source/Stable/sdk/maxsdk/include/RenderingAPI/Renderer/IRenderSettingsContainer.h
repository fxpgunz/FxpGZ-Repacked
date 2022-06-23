//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2014 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once 

#include <maxtypes.h>
#include <iparamb2Typedefs.h>

class Interval;
class IParamBlock2;
class ToneOperator;
class Renderer;

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class IRenderSettingsContainer
//
/*! Abstracts the set of settings and parameters which may affect rendering.

    This interface exposes commonly used render settings in a way that abstracts the differences between offline and interactive rendering.
    For offline rendering, most of these parameters are internally extracted from class RendParams. For interactive rendering, they are queried
    from the appropriate source.

    Note that many of the classical 3ds Max rendering settings, from the common render settings panel, are not exposed here. Those parameters are,
    for the most part, considered specific to the legacy scanline renderer and are generally ignored by the Rendering API.
*/
class IRenderSettingsContainer
{
public:

    enum class RenderTargetType;
    class IChangeNotifier;

    /*! Returns whether only selected objects should be rendered.
        \remark This options is, internally, taken into account by ISceneContainer::GetGeometricNodes(). */
    virtual bool GetRenderOnlySelected() const = 0;
    /* Returns whether hidden objects should be rendered. 
        \remark This options is, internally, taken into account by ISceneContainer::GetGeometricNodes(). */
    virtual bool GetRenderHiddenObjects() const = 0;
    /* Returns whether frozen objects should be considered hidden.
        \remark This options is, internally, taken into account by ISceneContainer::GetGeometricNodes(). */
    virtual bool GetHideFrozenObjects() const = 0;
    // Returns whether displacement should be enabled.
    virtual bool GetDisplacementEnabled() const = 0;
    /*! Returns the duration, in time ticks, of a single animation frame.
        This value is mostly useful for motion blur: it may be used to determine the time values for the shutter open and close times. 
        \remark Consider using ICameraContainer::GetGlobalMotionBlurSettings() to get shutter time values for motion blur. */
    virtual TimeValue GetFrameDuration() const = 0;
    //! Returns whether the session is rendering a preview for he material editor or browser.
    bool GetIsMEditRender() const;
    //! Returns the render target type for which the render session was created: offline, interactive, material editor, etc.
    virtual RenderTargetType GetRenderTargetType() const = 0;
    //! Returns the renderer from which the render session was created.
    virtual Renderer& GetRenderer() const = 0;

    //! \name Exposure Control
    //@{
    /*! Returns the ToneOperator (a.k.a. exposure control) plugin to be used by the render session.
        \remark Consider letting the system process the ToneOperator, via IRenderSessionContext::GetMainFrameBufferProcessor(). 
        \return The ToneOperator to be used by the render session, or null if none. */
    virtual ToneOperator* GetToneOperator() const = 0;
    /*! Returns the physical scale to be used.
        
        The physical scale is used to convert non-physical lighting quantities to physical ones, mainly textures used for environment lighting
        and self-illumination. For a self-illumination texture, the default physical scale of 1500 would map a white texel (RGB 1,1,1) to 1500&frasl;&Pi; cd/m<sup>2</sup> (477 cd&frasl;m<sup>2</sup>).
        
        Note that the physical scale works in a backwards manner: <b>rather than multiply non-physical values by the scale, physical values must be divide
        by the scale</b>. This brings all values into generic, non-physical space. Following rendering, the ToneOperator internally multiplies all values
        by the physical scale, thus bringing all values values back to physical space. This backward implementation is meant to ensure compatibility with
        legacy, non-physical plugins which do not know about the physical scale. A simple alternative is to divide the frame buffer values, 
        by the physical scale, before they are passed to the ToneOperator.

        \param t The time at which to evaluate the physical scale.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. 
        \return The physical scale to be used for converting non-physical lighting values to physical ones. */
    virtual float GetPhysicalScale(const TimeValue t, Interval& validity) const = 0;
    //@}


    //! \name Notification
    //@{
    //! Registers a notifier callback, to be called when interactive changes occur.
    virtual void RegisterChangeNotifier(IChangeNotifier& notifier) const = 0;
    //! Unregisters a notification callback, previously registered with RegisterChangeNotifier().
    virtual void UnregisterChangeNotifier(IChangeNotifier& notifier) const = 0;
    //@}

protected:

    // Protected destructor forces using implementation for deletion.
    virtual ~IRenderSettingsContainer() {}
};

//==================================================================================================
// enum class IRenderSettingsContainer::RenderTargetType
//
//! The type of target for which the render session was created.
enum class IRenderSettingsContainer::RenderTargetType
{
    //! Preview render for the material editor or browser.
    MaterialEditor,
    //! Preview render for the tone operator preview panel. In this mode, the tone operator is applied as a post-process, 
    //! and IRenderSettingsContainer::GetToneOperator() will return null.
    ToneOperatorPreview,
    //! A standard offline render, for a still or a series of animation frames.
    AnimationOrStill,
    //! Interactive rendering (a.k.a. ActiveShade), where the renderer must react to changes made by the user while the renderer is active.
    Interactive
};

//==================================================================================================
// class IRenderSettingsContainer::IChangeNotifier
//
/*! Notification callback interface for IRenderSettingsContainer.

    When registered with an IRenderSettingsContainer, this interface will be called whenever a relevant change occurs.
*/
class IRenderSettingsContainer::IChangeNotifier
{
public:
    //! Notifies that the ToneOperator plugin was replaced by a different one (i.e. IRenderSettingsContainer::GetToneOperator() will return a different value).
    virtual void NotifyToneOperatorSwapped() = 0;
    //! Notifies that a parameter of the ToneOperator has changed, potentially meaning that the ToneOperator needs to be re-processed on the image being rendered.
    virtual void NotifyToneOperatorParamsChanged() = 0;
    //! Notifies that the physical scale has changed. \see IRenderSettingsContainer::GetPhysicalScale()
    virtual void NotifyPhysicalScaleChanged() = 0;
protected:
    //! Protected destructor prevents deletion through this interface
    virtual ~IChangeNotifier() {}
};

}}	// namespace

#include "IRenderSettingsContainer.inline.h"

