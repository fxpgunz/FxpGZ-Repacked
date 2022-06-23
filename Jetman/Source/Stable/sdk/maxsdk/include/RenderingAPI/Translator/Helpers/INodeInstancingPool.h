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

#include "../../RenderingAPI.h"

// Max SDK
#include <maxtypes.h>
#include <NotificationAPI/InteractiveRenderingAPI_Subscription.h>
#include <NotificationAPI/NotificationAPI_Events.h>
// std
#include <memory>

class INode;

namespace MaxSDK
{;
namespace RenderingAPI
{;
namespace TranslationHelpers
{;

//==================================================================================================
// class INodeInstancingPool
//
/*! Abstracts a set of nodes that all point to the the same object instance.

    This interface represents a set of nodes which, for rendering purposes, can be treated as all being instances of the same object. As such,
    the pool has a front-facing <i>representative node</i> which may be used to evaluate that shared/instanced object.
*/
class INodeInstancingPool
{
public:

    class IManager;
    class INotifier;

    /*! Returns the pool's <i>representative node</i>.
        The representative node is the front-facing node that may be used to evaluate the object that is instanced by the pool. In theory, any
        node in the pool may be used for this purpose; this method simply chooses one of them. */
    virtual INode* GetRepresentativeNode() const = 0;

    /*! Registers a change notification callback, which gets called whenever the pool, or the instanced object, changes.
        \param notifier The notification callback to be registered. */
    virtual void MonitorNodePool(INotifier& notifier) const = 0;
    /*! Unregisteres a notification callback previously registered with MonitorNodePool().
    \param notifier The notification callback to be unregistered. */
    virtual void StopMonitoringNodePool(INotifier& notifier) const = 0;

protected:

    // Protected destructor forces destruction through the shared_ptr returned by the manager.
    virtual ~INodeInstancingPool() {}
};

//==================================================================================================
// class INodeInstancingPool2
//
/*! Extends class INodeInstancingPool with additional functionality.

	See class INodeInstancingPool for more details.
*/
class INodeInstancingPool2 : 
    public INodeInstancingPool
{
public:

	/*! Returns the number of nodes in the pool.
		\remark In most cases, users of INodeInstancingPool should only care about the representative node, returned by GetRepresentativeNode().
		In some cases, however, they may care about the set of of nodes contained by the pool. Note, however, that nodes are only
		incorporated into their respective pool on calling INodeInstancingPool::IManager::GetPoolForNode(), and may be ejected at
		any time, e.g. when scene changes are detected. Querying the list of nodes contained by a pool is therefore only truly valid
		right after calling INodeInstancingPool::IManager::GetPoolForNode() for every node in the scene. */
    virtual size_t GetSize() const = 0;
	/*! Returns the node, at the given index, contained in the pool.
		\param index The index of the node to query; see GetSize().
		\remark In most cases, users of INodeInstancingPool should only care about the representative node, returned by GetRepresentativeNode().
		In some cases, however, they may care about the set of of nodes contained by the pool. Note, however, that nodes are only
		incorporated into their respective pool on calling INodeInstancingPool::IManager::GetPoolForNode(), and may be ejected at
		any time, e.g. when scene changes are detected. Querying the list of nodes contained by a pool is therefore only truly valid
		right after calling INodeInstancingPool::IManager::GetPoolForNode() for every node in the scene. */
    virtual INode* GetNode(const size_t index) const = 0;
};

//==================================================================================================
// class INodeInstancingPool::IManager
//
/*! Manages the creation and existence of node instancing pools.

    This manager is the interface through which new node pools are created. It also handles the addition of new nodes to existing pools.
*/
class INodeInstancingPool::IManager :
    public MaxHeapOperators
{
public:

    /*! Allocates a new instance of this manager interface.
        The returned interface becomes the exclusive ownership of the caller. 
        \param notification_client An interface to the Notification System, which will be used to monitor the pooled nodes, such that they may
            be automatically maintained correctly if the nodes should change in any way. This interface is optional; passing null will result
            in the monitoring capabilities to be disabled - hence the pools will not stay up-to-date, should the scene change in any way. */
    static RenderingAPIExport std::unique_ptr<IManager> AllocateInstance(NotificationAPI::IImmediateInteractiveRenderingClient* notification_client);

    virtual ~IManager() {}

    /*! Returns the instancing pool associated with the given node.
        This method first attempts to find an already-existing pool in which the given node could participate. If one cannot be found, a new one
        is created. The method uses a shared_ptr to facilitate lifetime management and garbage collections of the pools, such that the caller
        may hold onto the pool, without fear of it getting destroyed if its contents were to change and become empty. Note, however, that the contents
        of the pool could indeed change; INodeInstancingPool::MonitorNodePool() may be used to catch such changes.
        \param node The node for which to fetch the pool.
        \param t The time at which the node is to be evaluated. Nodes need to be evaluated to find out what object they point to. 
        \param nodeNotificationType The notifier type to be used when monitoring this node, as defined by the Notification System.
        \return The pool to which this node belongs. */
    virtual std::shared_ptr<const INodeInstancingPool> GetPoolForNode(INode& node, const TimeValue t, const NotificationAPI::NotifierType nodeNotificationType) = 0;
};

//==================================================================================================
// class INodeInstancingPool::INotifier
//
/*! Notification callback for monitoring changes in a node instancing pool.
*/
class INodeInstancingPool::INotifier
{
public:

    /*! Notifies that the object, being instanced by the pool, has changed and needs to be re-translated.
        For example, would be called if the mesh of the instanced object were to change.
        \param node_pool The pool which is sending the notification.
        \param event_type The type of event that triggered this notification. */
    virtual void NotifyPoolInstancedObjectChanged(const INodeInstancingPool& node_pool, const NotificationAPI::NodeEventType event_type) = 0;

    /*! Notifies that the set of nodes, contained in the pool, <i>may</i> have changed. 
        This method notifies that some nodes may have been added or removed to/from the pool. We say <i>may</i> have changed, because it's impossible
        to know for sure at this point due to node evaluations being disallowed (unsafe) in notification callbacks. The listener should use a
        deferred validity check (i.e. call Translator::Invalidte(true)) and later re-acquire the pool for the node being cared for. This
        re-acquisition of the pool will cause the nodes to be re-evaluated and the pools to be re-built accordingly.
        \param node_pool The pool which is sending the notification. */
    virtual void NotifyPoolContentMaybeChanged(const INodeInstancingPool& node_pool) = 0;
};

}}}	// namespace 

