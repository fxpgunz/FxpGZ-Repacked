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

/** \file InteractiveRenderingAPI_subscription.h

		This file contains The interactive rendering API classes to subscribe to events for interactive rendering.
        They are a high level API using the Notification API for Interactive Rendering
*/

#include "NotificationAPI_Subscription.h"

namespace MaxSDK
{;
namespace NotificationAPI
{;

class IInteractiveRenderingClient;
class IImmediateInteractiveRenderingClient;
class IOnDemandInteractiveRenderingClient;

//! IInteractiveRenderingManager class is the entry point of the Interactive Rendering API, you can create or remove a Interactive Rendering Client
//! Interactive rendering API is a middle API that makes the link between the low level NotificationAPI and the customer. It takes care of the lock/unlock workflow for active shade viewports
//! It is dedicated for interactive rendering.If the user switches to another viewport, the new viewport is automatically monitored, you don't have to deal with all this.
class IInteractiveRenderingManager
{
public:
    virtual ~IInteractiveRenderingManager(){}//!< Destructor

     /** GetManager static function
    * @return a pointer on IInteractiveRenderingManager
	*/
    static NotificationAPIExport IInteractiveRenderingManager* GetManager();

    /** Use this function to create an immediate client and get notified as soon as a change happens
    We use the Immediate Notification client you pass as a parameter.
    * @param notificationClient : a Notification client used to get notifications
	* @param version : may pass the version of Interactive Rendering client you want to use, -1 = latest.
    * @return a pointer on IImmediateInteractiveRenderingClient
	*/
    //!< 
	virtual IImmediateInteractiveRenderingClient* RegisterNewImmediateClient(IImmediateNotificationClient* notificationClient, int version = -1) = 0;

    /** Use this function to create an on demand client and ask for the changes when you need to update
    * We are using an ImmediateNotification client to get notifications from NotificationAPI and we sort and store the events until you ask for them
	* @param version : may pass the version of Interactive Rendering client you want to use, -1 = latest.
    * @return a pointer on IOnDemandInteractiveRenderingClient
	*/
    //!< 
	virtual IOnDemandInteractiveRenderingClient* RegisterNewOnDemandClient(int version = -1) = 0;

    /** RemoveClient function, to stop monitoring. Cleaning is done by the manager which means your <B>client gets deleted by the manager</B>.
	* @param client : the Interactive Rendering client you want to unregister.
    * @return true if succeeded, false if the client was not found
	*/
    //!< 
	virtual bool RemoveClient(IInteractiveRenderingClient* client) = 0;
        
    /** NumClients function
    * @return the number of Interactive Rendering clients connected
	*/
    //!< 
    virtual size_t NumClients()const = 0;

    /** GetClient function
	* @param index : the index of the Interactive Rendering clients to retrieve
    * @return a pointer on a IInteractiveRenderingClient or NULL if index is out of range
	*/
    //!< 
    virtual const IInteractiveRenderingClient* GetClient(size_t index)const = 0;

    /** DebugPrintToFile function
	* @param file : a valid FILE pointer to print debug information to
	*/
    //!< 
    virtual void DebugPrintToFile(FILE* file)const = 0;
};

/**
InteractiveRenderingCallback is the the class to derive from to create a callback to receive events from InteractiveRendering.
*/
class IInteractiveRenderingCallback
{
public:
    virtual ~IInteractiveRenderingCallback() {}
		
	/** Called to process an event.
	* @param viewEvent: the event in question
    * @param userData: the user data passed to the Monitor*() call on notification client. */
	virtual void InteractiveRenderingCallback_NotifyEvent(const IViewEvent& viewEvent, void* userData) = 0;
};

/** Base class for Interactive Rendering clients
*/
class IInteractiveRenderingClient
{
public:
    virtual ~IInteractiveRenderingClient(){}//!< Destructor
    
    virtual int                     VersionNumber() const= 0;//!< returns the version number of the notification client
    
    /** Print what's monitored inside the client (debugging purpose)
	* @param theFile a FILE you use to print out the data in it (can't be NULL)
	*/
	virtual void DebugPrintToFile(FILE* theFile)const=0;
};

//! Immediate client is to get notifications as soon as they happen (by opposite to OnDemand client)
class IImmediateInteractiveRenderingClient : 
    public IInteractiveRenderingClient,
    public NotificationAPI::IImmediateNotificationClient
{
public:
    virtual ~IImmediateInteractiveRenderingClient(){}//!< Destructor

    /** Monitor the active view whatever it is : a camera, a viewport or a light. This takes care of the lock/unlock view workflow for active shade.
    Also when the active view is changed to another view, you are called back and the new view is monitored automatically
    With this monitoring, you will events from notifierType : NotifierType_View and IViewEvent events
    * @param callback The interface used to notify of new events.
    * @param userData An arbitrary pointer which is passed back, with the event, through INotificationCallback.
    * @param bIsActiveShadeRunningInAViewport Indicates if we are running active shade in a viewport, we need to know that to lock the view to that viewport in that case
	* @return true if the active view is now monitored, false there was a problem
	*/
	virtual bool MonitorActiveShadeView(IInteractiveRenderingCallback& callback, void* userData, bool bIsActiveShadeRunningInAViewport = false) =0;

    /** Stop monitoring active view
    * @param callback The notification callback passed to the Monitor method.
    * @param userData The user data passed to the Monitor method.
	* @return true if the active view is no longer monitored now, false if there was a problem
	*/
    virtual bool StopMonitoringActiveView(IInteractiveRenderingCallback& callback, void* userData)=0;
};

//! Is a client where you get the notifications on demand, they are stored until you ask for them
class IOnDemandInteractiveRenderingClient : public IInteractiveRenderingClient
{
public:
    virtual ~IOnDemandInteractiveRenderingClient(){}//!< Destructor

    /** Returns the number of events currently waiting to be processed.
	*/
    virtual size_t NumberOfQueuedEvents() = 0;

    /** This function processes all events currently waiting in the queue. The queue is emptied in the process.
	* @param notificationCallback: The callback through which the events are processed, one by one.
	*/
    virtual void ProcessEvents(IInteractiveRenderingCallback& notificationCallback) = 0;

     /** Monitor the active view whatever it is : a camera, a viewport or a light. This takes care of the lock/unlock view workflow for active shade.
    * Also when the active view is changed to another view, the event is stored until you ask for the updates and the new view is monitored automatically
	* @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
    * @param bIsActiveShadeRunningInAViewport tells if we are running active shade in a viewport, we need to know that to lock the view to that viewport
	* @return true if the active view is now monitored, false there was a problem
	*/
	virtual bool MonitorActiveShadeView(void* userData, bool bIsActiveShadeRunningInAViewport = false)=0;

    /** Stop monitoring active view
    * @param userData the user data passed to the Monitor method.
	* @return true if the active view is no longer monitored now, false if there was a problem
	*/
    virtual bool StopMonitoringActiveView(void* userData)=0;
};

}}; //end of namespace 