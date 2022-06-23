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

// Max SDK
#include "../RenderingAPI.h"
#include <strbasic.h>
#include <MaxHeap.h>
#include <Rendering/IRenderMessageManager.h>
// std
#include <memory>

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class IRenderingLogger
//
/*! The Rendering API's interface for logging messages against 3ds Max build-in mechanisms.
    
    This interface abstracts 3ds Max's internal message logging interfaces for the needs of a renderer. Internally, it logs the messages to the
    render message window, as well as to file, as necessary.
*/
class IRenderingLogger :
    public MaxHeapOperators
{
public:

    /*! Returns a new instance of this interface.
        In standard cases, this logging interface is created by the system and passed to the render session through IRenderSessionContext. But,
        should a renderer need access to the logger outside the bounds of a render session, or should it be needed by a renderer that does not
        derive from UnifiedRenderer, then a logger can be obtained through this method.
        \param message_source The source of the messages, which determines which tab of the render message window receives the message.
        \return A new instance of a rendering logger, which is then owned (and must be deleted) by the caller. */
    static RenderingAPIExport std::unique_ptr<IRenderingLogger> AllocateInstance(const IRenderMessageManager::MessageSource message_source);

    //! The message type is used to categorize and filter messages, according to user preferences.
    enum class MessageType
    {
        //! A message related to reporting progress information (usually as a percentage or count).
        Progress,
        //! A message that provides useful information about the sate of the renderer, such as processing times.
        Info,
        //! A message which flags a potential issue in rendering, but an issue which may not necessarily result in artifacts or otherwise undesirable results.
        Warning,
        //! A message which flags an error, an issue which either prevents the renderer from continuing or which will almost certainly cause the rendered
        //! image to contain artifacts or undesirable/unexpected results. Internally, these errors are reported as SYSLOG_WARN, and as such will not
        //! cause a network render to automatically abort.
        Error,
        //! A debug message, which may be output to file, but which is typically not shown to the user.
        Debug
    };

    /*! Logs a message.
        \param type The type/category of the message.
        \param message The actual message. */
    virtual void LogMessage(const MessageType type, const MCHAR* message) = 0;

    /*! Logs a message which should only reported once per render session.
        Certain errors or events may be encountered numerous times during a translation or rendering process, and it may be undesirable to spam
        the user with identical, duplicate (error) messages. This method keeps a record of all the messages it has processed, ignoring all duplicates.
        \remark To reset the state of the logger, i.e. enable reporting duplicate messages again, this interface must be destroyed and re-created.
        \param type The type/category of the message.
        \param message The actual message. */
    virtual void LogMessageOnce(const MessageType type, const MCHAR* message) = 0;

    virtual ~IRenderingLogger() {}
};

}}	// namespace 

 