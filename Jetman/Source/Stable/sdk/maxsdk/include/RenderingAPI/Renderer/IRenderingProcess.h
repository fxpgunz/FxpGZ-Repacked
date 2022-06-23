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

#include <strbasic.h>
#include <strclass.h>

class Box2;
class INode;

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class IRenderingProcess
//
/*! The Rendering API's interface for managing the render process.

    This interface abstracts various tasks related to the rendering process:
        \li Progress reporting
        \li Event reporting and logging
        \li Abort management
        \li Running jobs from the main thread
        \li Reporting scene statistics

    The interface's implementation of these tasks is specialized for the rendering API, and should be used rather than existing (more general)
    3ds Max SDK equivalents (such as RendProgressCallback).
*/
class IRenderingProcess
{
public:

    class IMainThreadJob;
    class NamedTimerGuard;

    enum class ProgressType
    {
        Translation,
        Rendering
    };

    //! \name Progress Reporting
    //@{
    /*! Sets the progress title, displayed in the render progress dialog.
        \remark It's a good idea to reset the progress title (to an empty string) once a task is done - to avoid lingering, invalid titles.
        \remark The title string is also passed to the message logging system.
        \remark This method may safe be called <b>from any thread</b>, but not concurrently as it contains no locking.
        \remark This method must not be used for reporting while scene translation is in progress. Instead, use class ITranslationManager. 
        \param title The string to display as the progress title. */
    virtual void SetRenderingProgressTitle(const MCHAR* title) = 0;
    /*! Sets the numerical progress to be displayed in the progress dialog.
        \remark The progress is not reported to the logging system, so it is a good idea for the render session to also report progress messages.
        \remark This method may safe be called <b>from any thread</b>, but not concurrently as it contains no locking.
        \param done The number of units of work which are done. Current progress is expressed as: <i>done / total</i>.
        \param total The number of units of work in total. Current progress is expressed as: <i>done / total</i>.
        \param progressType The type of progress being reported. The system may report both types of progress differently. 
        \remark This method must not be used for reporting while scene translation is in progress. Instead, use class ITranslationManager. */
    virtual void SetRenderingProgress(const size_t done, const size_t total, const ProgressType progressType) = 0;
    /*! Reports progress for an infinite render.
        This method is used to report the number of work units performed, when there is no set limit - i.e. the renderer will progress indefinitely,
        until explicitly stopped by the user. The progress dialog will set itself up with an infinite/ongoing progress bar.
        \param done The number of units of work which are done.
        \param progressType The type of progress being reported. The system may report both types of progress differently. 
        \remark This method must not be used for reporting while scene translation is in progress. Instead, use class ITranslationManager. */
    virtual void SetInfiniteProgress(const size_t done, const ProgressType progressType) = 0;
    //@}

    //! \name Scene Reporting & Statistics
    //@{
    /*! Sets the scene statistics which get displayed in the render progress dialog.
        See RendProgressCallback::SetSceneStats(). */
    virtual void SetSceneStats(int nlights, int nrayTraced, int nshadowed, int nobj, int nfaces) = 0;
    /*! Reports a UVW channel is required but missing on the given geometric object.
        A translator may determine that it requires a certain UVW channel to be present (because a texture may be dependent on it). 
        If the channel is not present on the mesh, then it should report it using this method. The system will automatically report
        the error using the standard mechanisms - though only for offline rendering (MEdit and ActiveShade do not report an error).
        \param node The node which references the mesh for which a UVW channel is missing.
        \param channel_id The ID for the UVW channel which is missing. */
    virtual void ReportMissingUVWChannel(INode& node, const unsigned int channel_id) = 0;
    //@}

    //! \name Abort Management
    //@{
    /*! Returns whether the user has requested that the renderer abort.
        This method is to be polled at regular intervals by the renderer to determine whether the user has requested that the renderer abort.
        If it returns true, then the renderer should terminate as soon as possible, regardless of how far the rendering process has progressed.
        It is perfectly fine to have a partial or otherwise invalid frame buffer when aborting.
        \remark Once a render session has been aborted, that session will no longer be used for rendering: it is destroyed. It is therefore 
            OK for the render session to assume it will no longer be used. 
        \remark If the renderer has been paused, then this method may block until the renderer is resumed. This is the legacy, passive method
            for pausing a renderer.
        \return True if the an abort has been requested. False otherwise. */
    virtual bool HasAbortBeenRequested() = 0;
    //@}

    //! \name Event Logging
    //! These methods implement events which are used by the system for logging and reporting timing statistics.
    //@{
    /*! To be called when the render session starts translating the scene.
        This method reports a progress message and starts a timer. */
    virtual void TranslationStarted() = 0;
    /*! To be called when the render session finishes translating the scene.
        This method reports a progress message and the total time used for translating the scene. */
    virtual void TranslationFinished() = 0;
    /*! To be called when the render session starts rendering the scene.
        This method reports a progress message and starts a timer. 
        \remark This method should also be called if the renderer is restarted, such as following a scene update. */
    virtual void RenderingStarted() = 0; 
    /*! To be called by the render session when it effectively pauses the renderer.
        This method reports a progress message and pauses the rendering timer. 
        \remark This method does nothing if the renderer is already paused. */
    virtual void RenderingPaused() = 0;     
    /*! To be called by the render session when it effectively resumes the renderer, following a pause.
        This method reports a progress message and resumes the rendering timer.
        \remark This method does nothing if the renderer is not already paused. */
    virtual void RenderingResumed() = 0;
    /*! To be called when the render session finishes, aborts, or stops rendering the scene.
        This method reports a progress message and the total time used for rendering.
        \remark This method does nothing if the render was not currently running (paused is considered running). */
    virtual void RenderingFinished() = 0;
    /*! Returns the elapsed translation time, as delimited by calls to TranslationStarted() and TranslationFinished().
        \remark May be called while tranlsation is active (i.e. before TranslationFinished() has been called).
        \return The elapsed translation time, in seconds. */
    virtual float GetTranslationTimeInSeconds() const = 0;
    /*! Returns the elapsed rendering time, as delimited by calls to RenderingStarted() and RenderingFinished().
        \remark May be called while rendering is active (i.e. before RenderingFinished() has been called).
        \return The elapsed rendering time, in seconds. */
    virtual float GetRenderingTimeInSeconds() const = 0;
    //@}

    //! \name Timing Statistics
    //! These method are used to register timing statistics, associating them with a name that may be reported
    //! to the user at the end of the rendering operation.
    //@{
    //! \brief Starts the timer with the given name.
    /*! Creates a new timer with the given name, and starts that timer. If a timer with that name already exists,
        then it is resumed (un-paused). 
        \param timer_name The name of the timer, which will be reported to the user (through the log system) at the end
            of the rendering operation. An empty name causes the call to no-op. 
        \remark The timer may be sopped/paused by calling StopTimer() with the same timer name. 
        \remark Consider using class NamedTimerGuard to more easily start and stop timers. */
    virtual void StartTimer(const MSTR& timer_name) = 0;
    //! \brief Stops the timer with the given name.
    /*! Stops the existing timer with the given name. 
        \param timer_name The name of the timer, which will be reported to the user (through the log system) at the end
            of the rendering operation. An empty name causes the call to no-op. 
        \remark The time is actually paused, and may be re-started by calling StartTimer() again with the same timer name. */
    virtual void StopTimer(const MSTR& timer_name) = 0;
    //! \brief Returns the total time accumulated for the timer with the given name.
    /*! \param timer_name The name of the timer for which to retrieve the running timer. 
        \remark Calling this method on a timer which is currently running is perfectly valid; the current running time will be correctly queried. */
    virtual float GetTimerElapsedSeconds(const MCHAR* timer_name) const = 0;
    //@}

    //! \name Running Jobs from the Main Thread
    //@{
    /*! Runs a job from the main thread.
        The 3ds Max SDK is generally not thread safe. Most SDK operations, such as scene translation/update, must therefore be done from the main
        thread. Because the render session will not necessarily operate from the main thread (e.g. IInteractiveRenderSession::UpdateInteractiveSession()),
        this method provides a simple mechanism for execute an arbitrary job from the main thread.
        \remark This call is synchronous: it returns once the job has completed or has been aborted.
        \remark If the render session is aborted, then pending jobs are cancelled and this method returns false.
        \remark If this method is called from the main thread, then the job is immediately executed with no synchronization overhead.
        \return True if the job executed successfully, false if the job didn't execute because the render session was aborted. */
    virtual bool RunJobFromMainThread(IMainThreadJob& job) = 0;
    //@}

protected:

    //! Protected destructor prevents deletion through this interface.
    virtual ~IRenderingProcess() {}
};

//==================================================================================================
// class IRenderingProcess::IMainThreadJob
//
/*! Interface to a job which gets executed from the main thread.

    Main thread jobs are run using IRenderingProcess::RunJobFromMainThread().
*/
class IRenderingProcess::IMainThreadJob
{
public:

    //! Runs the job, guaranteed to be called from the main thread.
    virtual void ExecuteMainThreadJob() = 0;

protected:

    //! Protected destructor prevents deletion through this interface.
    virtual ~IMainThreadJob() {}
};

//==================================================================================================
// class IRenderingProcess::NamedTimerGuard
//
/*! \brief Guard class for starting and stopping a timer automatically.
    
    This class calls IRenderingProcess::StartTimer() in the constructor, and IRenderProcess::StopTimer() in the destructor
    when in standard mode. But the behaviour may be reversed to instead call StopTimer() in the constructor and StartTimer()
    in the destructor, effectively pausing the timer for the lifetime of the guard.
*/
class IRenderingProcess::NamedTimerGuard :
    public MaxSDK::Util::Noncopyable
{
public:

    //! \brief Starts or stops the given timer.
    /*! \param timer_name The name of the timer to be started or stopped.
        \param rendering_process The rendering process interface through which the timer is started or stopped.
        \param pause When false, the timer is started in the constructor and stopped in the destructor.
            When true, the behaviour is reversed: the timer is stopped in the constructor and started in the destructor.
        \sa IRenderingProcess::StartTimer(), IRenderingProcess::StopTimer() */
    NamedTimerGuard(const MSTR& timer_name, IRenderingProcess& rendering_process, const bool pause = false);
    //! \brief Stops re re-starts the timer, depending on the parameters passed to the constructor.
    ~NamedTimerGuard();

private:

    const bool m_pause;
    const MSTR m_timer_name;
    IRenderingProcess& m_rendering_process;
};

}}	// namespace 

#include "IRenderingProcess.inline.h"
