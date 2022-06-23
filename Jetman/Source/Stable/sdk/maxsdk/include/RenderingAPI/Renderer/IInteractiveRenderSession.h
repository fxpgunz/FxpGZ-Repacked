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

namespace MaxSDK
{;
namespace RenderingAPI
{;

class IRenderingProcess;

//==================================================================================================
// class IInteractiveRenderSession
//
/*! An interface which encapsulates a render session for interactive rendering.

    This render session is allocated for the duration of an ActiveShade session. It is created when the user initiates the ActiveShade render,
    and destroyed when the user terminates the ActiveShade process; these correspond roughly to calls to IInteractiveRenderer::BeginSession and
    IInteractiveRenderer::EndSession.
*/
class IInteractiveRenderSession :
    public IRenderSession
{
public:

    /*! Initializes the interactive rendering session.
        This method is called once, <b>from the main thread</b>, just after the IInteractiveRenderSession is instantiated. The implementation
        should perform initial setup of the renderer, for example the initial translation of the scene. If any dialogs or pop-ups need to be
        shown to the user in case of scene errors, missing files, etc. - this is the place to do it. Modal interruptions of the interactive workflow
        should not occur beyond this initialization phase.
        \remark This method may start the renderer, but should neither block nor wait for any reason. It should not perform frame buffer updates.
        \param t The time at which the scene is to be translated.
        \return True on success. False if initialize fails, causing the render session to be aborted and destroyed. */
    virtual bool InitiateInteractiveSession(const TimeValue t) = 0;

    /*! Updates the interactive session at regular intervals.
        This method is polled continuously to update the render session. This update normally consists of:
            \li Update (re-translate) any part of the scene which might have been modified by the user since the last update, or which might not be
                valid at the new time value. Note that the renderer should re-start in this case.
            \li Update the frame buffer if required (see ICameraContainer::Bitmap()).

        \remark This method is called from a dedicated thread - <b>not from the main thread</b>. It's therefore unsafe to access the 3ds Max scene in any way.
            To perform tasks which require the main thread, use IRenderingProcess::RunJobFromMainThread().
        \remark This method is polled continuously, without any explicit wait between calls. It's the implementation's responsibility to wait for some milliseconds
            if it has nothing to do. Failing to do so will cause the thread to run at 100% usage and will impact rendering performance.
        \param t The time at which the scene should be updated. This could differ from either that passed to the last call to this method, or to InitializeInteractiveSession().
            A time change should be interpreted as the user moving the time slider. The render session must then update any scene element which is invalid at the new time.
        \param doneRendering A boolean which must be set to <i>true</i>, by this method's implementation, if rendering has finished. The state of
            being <i>done</i> is reversible: this method will continue to be called after the renderer is done, and the renderer is allowed to react 
            to scene changes and restart rendering (at which point it will stop reporting itself as <i>done</i>). There are mechanisms that rely on 
            interactive renderers correctly reporting when they are done - for example a script that would save the image once it's fully rendered.
        \return True on success. False on error of any kind, causing the render session to be aborted. */
    virtual bool UpdateInteractiveSession(const TimeValue t, bool& doneRendering) = 0;

    /*! Requests that the rendering process be aborted as the session is being terminated.
        This method is called when the session needs to be terminated, just before the IInteractiveRenderSession is destroyed. 
        \remark UpdateInteractiveSession() is guaranteed never to be called either during or after this call.
        \remark This method is called from the main thread. */
    virtual void TerminateInteractiveSession() = 0;

    /*! Returns whether there are interactive changes which have not yet been taken into account by the renderer
        This state may be used, for example, by scripts which wish to wait until rendering is done (see UpdateInteractiveSession()) <b>and</b>
        all pending scene changes have been processed, before they save the rendered image.
        \remark This is called from the main thread, and as such may be called concurrently with UpdateInteractiveSession(). It should therefore be 
            thread safe. 
        \param t The time at which to check for scene validity. If the scene was updated/translated at a different time value, then it might not
            be valid for this one. */
    virtual bool IsInteractiveSessionUpToDate(const TimeValue t) = 0;
};

}}	// namespace 

