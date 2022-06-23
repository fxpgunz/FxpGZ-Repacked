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

/** \file NotificationAPI_Subscription.h

		This file contains the notification API classes to subscribe to scene/nodes events.
*/

#include "NotificationAPI_Events.h"

/**
The Notification system API for internal / 3rd parties plugins
This is to avoid the NotifyRefChanged sorting of the messages and filtering node and scene events, this should be done in this 
module and the plugin gets notified directly without having to monitor all messages and sort them out.
It's a single entry point for all the notification systems we have in 3ds Max.
*/

#ifdef NOTIFICATION_SYSTEM_MODULE
    #define NotificationAPIExport __declspec(dllexport)
#else
    #define NotificationAPIExport __declspec(dllimport)
#endif

class ReferenceTarget;

namespace MaxSDK
{;
namespace NotificationAPI
{;
	class INotificationClient;
    class IImmediateNotificationClient;
    class IOnDemandNotificationClient;

    //! INotificationManager class is the entry point of the Notification API, you can create or remove a Notification Client
	class INotificationManager
	{
	public:
        virtual ~INotificationManager(){}//!< Destructor

        /** GetManager static function
        * @return a pointer on INotificationManager
		*/
        static NotificationAPIExport INotificationManager* GetManager();

        /** Use this function to create a client which is the way to monitor notifications on nodes, materials, texmaps etc.
        * and get notified immediately as soon as a change happens
		* @param version : may pass the version of notification client you want to use, -1 = latest.
        * @return a pointer on INotificationClient
		*/
        //!< 
		virtual IImmediateNotificationClient* RegisterNewImmediateClient(int version = -1) = 0;

        /** Use this function to create a client which is the way to monitor notifications on nodes, materials, texmaps etc with on demand mode changes.
		* @param version : may pass the version of notification client you want to use, -1 = latest.
        * @return a pointer on INotificationClient
		*/
        //!< 
		virtual IOnDemandNotificationClient* RegisterNewOnDemandClient(int version = -1) = 0;

        /** RemoveClient function, to stop monitoring. Cleaning is done by the manager.
		* @param client : the notification client you want to unregister.
        * @return true if succeeded, false if the client was not found
		*/
        //!< 
		virtual bool RemoveClient(INotificationClient* client) = 0;
        
        /** NumNotificationClients function
        * @return the number of notification clients connected
		*/
        //!< 
        virtual size_t NumClients()const = 0;

        /** GetNotificationClient function
		* @param index : the index of the Notification clients to retrieve
        * @return a pointer on a INotificationClient or NULL if index is out of range
		*/
        //!< 
        virtual const INotificationClient* GetClient(size_t index)const = 0;

        /** DebugPrintToFile function
		* @param file : a valid FILE pointer to print debug information to : all the notifiers, listeners and what they monitor
		*/
        //!< 
        virtual void DebugPrintToFile(FILE* file)const = 0;

        /** Notifies the system of a render settings event.
            This method is used internally by the the application to broadcast a change in the 
            scene's render settings. The notification system relies on this method to catch such
            events because there are no other, pre-existing methods (such as through BroadcastNotification())
            for broadcasting such events.
            @param eventType: The type of event to be broadcast. */
        virtual void NotifyRenderSettingsEvent(const RenderSettingsEventType eventType) const = 0;
        /** Notifies the system of a render environment event.
            This method is used internally by the the application to broadcast a change in the 
            scene's render environment. The notification system relies on this method to catch such
            events because there are no other, pre-existing methods (such as through BroadcastNotification())
            for broadcasting such events.
            @param eventType: The type of event to be broadcast. */
        virtual void NotifyEnvironmentEvent(const RenderEnvironmentEventType eventType) const = 0;
	};

	class IGenericEvent;

	//! The notification callback used to process all events from the notification API.
	class INotificationCallback
	{
	public:
		
		/** Called to process an event.
          Note that it is possible to monitor and stop monitoring events from within this notification callback. The notification system
          is specifically built to handle that.
		* @param genericEvent: the event in question
        * @param userData: the user data passed to the Monitor*() call on notification client.
		*/
		virtual void NotificationCallback_NotifyEvent(const IGenericEvent& genericEvent, void* userData) = 0; 

    protected:

        // Protected destructor ensures deletion cannot happen through this interface.
        virtual ~INotificationCallback() {}
	};

    //! Base class for a notification client, is used to subscribe to notification events by monitoring 3ds Max data
	class INotificationClient
	{
	public:
        virtual ~INotificationClient(){};//!< Destructor
        virtual int                     VersionNumber() const= 0;//!< returns the version number of the notification client
    	
        /** Enable notifications
        * @param enable : boolean to tell if we should receive notifications or not.
        */
		virtual void EnableNotifications(bool enable)=0;

        /** Check if notifications are enabled or disabled
        * @return true if they are enabled
        */
		virtual bool NotificationsEnabled(void)const=0;

		/** Print what's monitored inside the client (debugging purpose)
		* @param theFile a FILE you use to print out the data in it (can't be NULL)
		*/
		virtual void DebugPrintToFile(FILE* theFile)const=0;
	};

	//! Immediate client is to get notifications as soon as they happen (by opposite to OnDemand client)
	class IImmediateNotificationClient : public INotificationClient
	{
	public:
        virtual ~IImmediateNotificationClient(){};//!< Destructor

        /** Monitor this node events
		* @param node the INode* to monitor.
		* @param type the NotifierType for this node.
        * @param monitoredEvents : bit array flags, the events you want to monitor for that node, you'll get called only called for the selected events. Example for 2 events is : UpdateType_Node_WireColor | UpdateType_Node_Transform;
        * @param callback the interface used to notify of new events.
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
		* @return true if the node is now monitored, false there was a problem
		*/
		virtual bool MonitorNode(INode& node, NotifierType type, size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;
		
        /** Monitor this material events
		* @param mtl the Mtl* to monitor.
        * @param monitoredEvents : bit array that is the events flags you want to monitor for that material, you'll get called only for the selected events
        * @param callback the interface used to notify of new events.
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
		* @return true if the material is now monitored, false if a problem occurred
		*/
		virtual bool MonitorMaterial(Mtl& mtl, size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;
		
		/** Monitor this Texmap events
		* @param texmap the Texmap to monitor.
        * @param monitoredEvents bit array : the events flags you want to monitor for that texmap, you'll get called only for the selected events
        * @param callback the interface used to notify of new events.
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
		* @return true if the texmap is now monitored, false if a problem occurred
		*/
		virtual bool MonitorTexmap(Texmap& texmap,	size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;

        /** Monitor events on a generic reference target.
          This is used to monitor notifications an an arbitrary ReferenceTarget class for which there doesn't exist a more specialized monitoring function
          (as there are for INode, Mtl, Texmap, etc.).
		* @param refTarg The reference target to be monitored.
        * @param monitoredEvents : bit array (see enum ReferenceTargetEventType) that is the events flags you want to monitor for that material, you'll get called only for the selected events
        * @param callback the interface used to notify of new events.
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
		* @return true if the reference target is now monitored, false if a problem occurred
		*/
		virtual bool MonitorReferenceTarget(ReferenceTarget& refTarg, size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;

        /** Monitor viewport events such as transform matrix changes (when you move the camera viewport while it's a viewport and not a Max camera)
		* @param viewID the viewport ID to monitor.
        * @param monitoredEvents bit array : the events flags you want to monitor for this viewport, you'll get called only for the selected events
        * @param callback the interface used to notify of new events.
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
        * @return true if the view is now monitored, false if a problem occurred
		*/
		virtual bool MonitorViewport(int viewID, size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;
		
        /** Monitor render environment events (Tone operator for example)
        * @param monitoredEvents bit array : the events flags you want to monitor for render environment, you'll get called only for the selected events
        * @param callback the interface used to notify of new events.
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
        * @return true if we monitor these events now, false if there was a problem
		*/
		virtual bool MonitorRenderEnvironment(size_t monitoredEvents, INotificationCallback& callback, void* userData)  = 0;

        /** Monitor render settings events (lock/unlock view for example)
        * @param monitoredEvents bit array : the events flags you want to monitor for render settings, you'll get called only for the selected events
        * @param callback the interface used to notify of new events.
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
        * @return true if we monitor these events now, false if there was a problem
		*/
        virtual bool MonitorRenderSettings(size_t monitoredEvents, INotificationCallback& callback, void* userData) = 0;

		/** Monitor scene events such as node added, file reset, file load, File import etc. 
        * @param monitoredEvents bit array : the events flags you want to monitor for the scene, you'll get called only for the selected events
        * @param callback the interface used to notify of new events.
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
        * @return true if we monitor these scene events now, false if there was a problem
		*/
		virtual bool MonitorScene(size_t monitoredEvents, INotificationCallback& callback, void* userData) = 0;

        /** Stop monitoring this node
        Unregisters a previously registered notification callback.
		* @param pNode the INode* to stop monitoring for events.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param callback the notification callback passed to the Monitor method.
        * @param userData the user data passed to the Monitor method.
		* @return true if the node is not monitored now, false if the node was not in our arrays
		*/
		virtual bool StopMonitoringNode(INode& pNode, size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;

		/** Stop monitoring this material
        Unregisters a previously registered notification callback.
		* @param pMtl the Mtl* to stop monitoring.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param callback the notification callback passed to the Monitor method.
        * @param userData the user data passed to the Monitor method.
		* @return true if the material is not monitored now, false if the material was not in our arrays
		*/
		virtual bool StopMonitoringMaterial(Mtl& pMtl, size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;

        /** Stop monitoring this texmap
        Unregisters a previously registered notification callback.
		* @param pTexmap the Texmap* to stop monitoring.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param callback the notification callback passed to the Monitor method.
        * @param userData the user data passed to the Monitor method.
		* @return true if the Texmap is not monitored now, false if the Texmap was not in our arrays
		*/
		virtual bool StopMonitoringTexmap(Texmap& pTexmap, size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;

        /** Stop monitoring a reference target
        Unregisters a previously registered notification callback.
		* @param refTarg The reference target to stop monitoring.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param callback the notification callback passed to the Monitor method.
        * @param userData the user data passed to the Monitor method.
		* @return true if the reference target is not monitored now, false if the Texmap was not in our arrays
		*/
		virtual bool StopMonitoringReferenceTarget(ReferenceTarget& refTarg, size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;

        /** Stop monitoring this viewport
        Unregisters a previously registered notification callback.
		* @param viewID the viewID of the viewport to stop monitoring.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param callback the notification callback passed to the Monitor method.
        * @param userData the user data passed to the Monitor method.
		* @return true if the view is not monitored now, false if the pView was not in our arrays
		*/
        virtual bool StopMonitoringViewport(int viewID, size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;

        /** Stop monitoring render environment
        Unregisters a previously registered notification callback.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param callback the notification callback passed to the Monitor method.
        * @param userData the user data passed to the Monitor method.
		* @return true if it is not monitored now, false if it was not monitored
		*/
        virtual bool StopMonitoringRenderEnvironment(size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;
		
        /** Stop monitoring render settings
        Unregisters a previously registered notification callback.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param callback the notification callback passed to the Monitor method.
        * @param userData the user data passed to the Monitor method.
		* @return true if it is not monitored now, false if it was not monitored
		*/
        virtual bool StopMonitoringRenderSettings(size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;

        /** Stop monitoring scene
        Unregisters a previously registered notification callback.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param callback the notification callback passed to the Monitor method.
        * @param userData the user data passed to the Monitor method.
		* @return true if these scene events are no longer monitored now, false if there was a problem
		*/
        virtual bool StopMonitoringScene(size_t monitoredEvents, INotificationCallback& callback, void* userData)=0;
	};

    //! Is a notification client where you get the notifications on demand, they are stored until you ask for them
	class IOnDemandNotificationClient : public INotificationClient
	{
	public:
        virtual ~IOnDemandNotificationClient(){};//!< Destructor

        /** Returns the number of events currently waiting to be processed.
		*/
        virtual size_t NumberOfQueuedEvents() = 0;

        /** This function processes all events currently waiting in the queue. The queue is emptied in the process.
		* @param notificationCallback: The callback through which the events are processed, one by one.
		*/
        virtual void ProcessEvents(INotificationCallback& notificationCallback) = 0;

		/** Monitor this node events
		* @param node the INode* to monitor.
		* @param type the NotifierType for this node.
        * @param monitoredEvents : bit array flags, the events you want to monitor for that node, you'll get called only called for the selected events. Example for 2 events is : UpdateType_Node_WireColor | UpdateType_Node_Transform;
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
		* @return true if the node is now monitored, false there was a problem
		*/
		virtual bool MonitorNode(INode& node, NotifierType type, size_t monitoredEvents, void* userData)=0;
		
        /** Monitor this material events
		* @param mtl the Mtl* to monitor.
        * @param monitoredEvents : bit array that is the events flags you want to monitor for that material, you'll get called only for the selected events
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
		* @return true if the material is now monitored, false if a problem occurred
		*/
		virtual bool MonitorMaterial(Mtl& mtl, size_t monitoredEvents, void* userData)=0;
		
		/** Monitor this Texmap events
		* @param texmap the Texmap to monitor.
        * @param monitoredEvents bit array : the events flags you want to monitor for that texmap, you'll get called only for the selected events
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
		* @return true if the texmap is now monitored, false if a problem occurred
		*/
		virtual bool MonitorTexmap(Texmap& texmap,size_t monitoredEvents, void* userData)=0;

        /** Monitor events on a generic reference target.
          This is used to monitor notifications an an arbitrary ReferenceTarget class for which there doesn't exist a more specialized monitoring function
          (as there are for INode, Mtl, Texmap, etc.).
		* @param refTarg The reference target to be monitored.
        * @param monitoredEvents : bit array (see enum ReferenceTargetEventType) that is the events flags you want to monitor for that material, you'll get called only for the selected events
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
		* @return true if the reference target is now monitored, false if a problem occurred
		*/
		virtual bool MonitorReferenceTarget(ReferenceTarget& refTarg, size_t monitoredEvents, void* userData)=0;

        /** Monitor viewport events such as transform matrix changes (when you move the camera viewport while it's a viewport and not a Max camera)
		* @param viewID the viewport ID to monitor.
        * @param monitoredEvents bit array : the events flags you want to monitor for this viewport, you'll get called only for the selected events
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
        * @return true if the view is now monitored, false if a problem occurred
		*/
		virtual bool MonitorViewport(int viewID, size_t monitoredEvents, void* userData)=0;
		
        /** Monitor render environment events (Tone operator for example)
        * @param monitoredEvents bit array : the events flags you want to monitor for render environment, you'll get called only for the selected events
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
        * @return true if we monitor these events now, false if there was a problem
		*/
		virtual bool MonitorRenderEnvironment(size_t monitoredEvents, void* userData)  =0;

        /** Monitor render environment events (Tone operator for example)
        * @param monitoredEvents bit array : the events flags you want to monitor for render settings, you'll get called only for the selected events
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
        * @return true if we monitor these events now, false if there was a problem
		*/
        virtual bool MonitorRenderSettings          (size_t monitoredEvents, void* userData) = 0;

        /** Monitor scene events such as node added, file reset, file load, File import etc. 
        * @param monitoredEvents bit array : the events flags you want to monitor for the scene, you'll get called only for the selected events
        * @param userData an arbitrary pointer which is passed back, with the event, through INotificationCallback.
        * @return true if we monitor these scene events now, false if there was a problem
		*/
		virtual bool MonitorScene(size_t monitoredEvents, void* userData)  =0;

                /** Stop monitoring this node
		* @param pNode the INode* to stop monitoring for events.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param userData the user data passed to the Monitor method.
		* @return true if the node is not monitored now, false if the node was not in our arrays
		*/
		virtual bool StopMonitoringNode(INode& pNode, size_t monitoredEvents, void* userData)=0;

		/** Stop monitoring this material
		* @param pMtl the Mtl* to stop monitoring.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param userData the user data passed to the Monitor method.
		* @return true if the material is not monitored now, false if the material was not in our arrays
		*/
		virtual bool StopMonitoringMaterial(Mtl& pMtl, size_t monitoredEvents, void* userData)=0;

        /** Stop monitoring this texmap
		* @param pTexmap the Texmap* to stop monitoring.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param userData the user data passed to the Monitor method.
		* @return true if the Texmap is not monitored now, false if the Texmap was not in our arrays
		*/
		virtual bool StopMonitoringTexmap(Texmap& pTexmap, size_t monitoredEvents, void* userData)=0;
		
        /** Stop monitoring a reference target
        Unregisters a previously registered notification callback.
		* @param refTarg The reference target to stop monitoring.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param userData the user data passed to the Monitor method.
		* @return true if the reference target is not monitored now, false if the Texmap was not in our arrays
		*/
		virtual bool StopMonitoringReferenceTarget(ReferenceTarget& refTarg, size_t monitoredEvents, void* userData)=0;

        /** Stop monitoring this viewport
		* @param viewID the viewID of the viewport to stop monitoring.
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param userData the user data passed to the Monitor method.
		* @return true if the view is not monitored now, false if the pView was not in our arrays
		*/
        virtual bool StopMonitoringViewport(int viewID, size_t monitoredEvents, void* userData)=0;

        /** Stop monitoring render environment
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param userData the user data passed to the Monitor method.
		* @return true if it is not monitored now, false if it was not monitored
		*/
        virtual bool StopMonitoringRenderEnvironment(size_t monitoredEvents, void* userData)=0;
		
        /** Stop monitoring render settings
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param userData the user data passed to the Monitor method.
		* @return true if it is not monitored now, false if it was not monitored
		*/
        virtual bool StopMonitoringRenderSettings(size_t monitoredEvents, void* userData)=0;

        /** Stop monitoring scene
        * @param monitoredEvents : bit array flags, the events you want to stop monitoring, if you remove all the events we had recorded, we'll delete our internal data
        * @param userData the user data passed to the Monitor method.
		* @return true if these scene events are no longer monitored now, false if there was a problem
		*/
        virtual bool StopMonitoringScene(size_t monitoredEvents, void* userData)=0;
	};

	
};//end of namespace NotificationAPI
};//end of namespace Max


