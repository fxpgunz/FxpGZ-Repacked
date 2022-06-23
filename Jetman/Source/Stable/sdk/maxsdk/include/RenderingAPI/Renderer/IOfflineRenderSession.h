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

#include "IRenderSession.h"

#include <maxtypes.h>

class IRenderElement;

namespace MaxSDK
{;
namespace RenderingAPI
{;

class IRenderingProcess;
class IFrameBufferProcessor;

//==================================================================================================
// class IOfflineRenderSession
//
/*! An interface which encapsulates a render session for offline (non-interactive) rendering.

    This render session is allocated for the duration of an offline rendering session, which consists of rendering one or more animation frames,
    without the possibility of the user interacting (modifying) with the scene while the renderer is active. This interactive is created when the
    user initiates an offline render, and destroyed when the rendering operation is completed; this corresponds roughly to calls to Renderer::Open()
    and Renderer::Close().
*/
class IOfflineRenderSession :
    public IRenderSession
{
public:

    /*! Translates or updates the scene at the given time.
        This method is called once per frame to translate (on first frame) or update (on further frames) the scene, in preparation for the frame
        about to be rendered. It's important that the scene translation be handled by this method (and not by RenderOfflineFrame()), because there
        are certain system events that must occur between translation and rendering. These system events are handled by the system.
        \param t The time at which the scene is to be translated.
        \return True on success. False on failure, for any reason - in which case the rendering process will be aborted. */
    virtual bool TranslateScene(const TimeValue t) = 0;

    /*! Renders a single frame of an animation sequence.
        This is called to render a single frame of an animation sequence, following a call to TranslateScene (not quite immediately, a few things
        occur in between). Before returning, this method is expected to update the frame buffer with the rendered result (see ICameraContainer::Bitmap()).
        \param t The time at which the frame is being rendered (same value passed to the previous call to TranslateScene()).
        \return True on success. False on failure, for any reason - in which case the rendering process will be aborted. */
    virtual bool RenderOfflineFrame(const TimeValue t) = 0;

    /*! Processes render elements, one by one, after a frame has been rendered.
        Following a successful call to RenderOfflineFrame(), the system iterates over all active render elements and calls this method for each.
        The implementation is expected to store the result, for each element is supports, using the provided frame buffer processor. Any unsupported
        render elements should simply be skipped, in which case the renderer may output an error message if it so desires.
        \remark The render session may skip this mechanism and process its render elements results in the call to RenderOfflineFrame(), if it wishes.
            It should, then, simply return <i>true</i> from this method. The render elements should then be fetched using IRenderSessionContext::GetRenderElements().
        \param t The time at which the frame is being rendered (same value passed to the previous call to RenderOfflineFrame()).
        \param render_element The render element to be processed.
        \param frame_buffer_processor The interface through which the frame buffer is to be stored. This is optional: the renderer may store the
            the result directly into the render element's Bitmap, if it wishes so.
        \return True on success. False on failure, for any reason - in which case the rendering process will be aborted. */
    virtual bool StoreRenderElementResult(const TimeValue t, IRenderElement& render_element, IFrameBufferProcessor& frame_buffer_processor) = 0;

    /*! Stops the progressive rendering process at the next possible iteration.
        See IStoppableRenderer::StopRendering(), from which this call is forwarded. */
    virtual void StopRendering() = 0;
    /*! Pauses the rendering process.
        See IStoppableRenderer::PauseRendering(), from which this call is forwarded. */
    virtual void PauseRendering() = 0;
    /*! Resumes the rendering process, following a pause.
        See IStoppableRenderer::ResumeRendering(), from which this call is forwarded. */
    virtual void ResumeRendering() = 0;
};

}}	// namespace 

