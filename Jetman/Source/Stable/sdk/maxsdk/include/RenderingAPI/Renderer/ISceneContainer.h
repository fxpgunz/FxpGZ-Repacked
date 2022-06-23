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

#include <Noncopyable.h>
#include <maxtypes.h>
#include <box3.h>

#include <vector>

class INode;
class Interval;

namespace MaxSDK
{;
namespace RenderingAPI
{;

class ICameraContainer;
class IRenderSettingsContainer;

//==================================================================================================
// class ISceneContainer
//
/*! Abstracts the contents of the scene to be rendered.

    This interface exposes methods for retrieving the contents of the scene, the set of nodes to be rendered.
*/
class ISceneContainer
{
public:

    class IChangeNotifier;

    /*! Returns the root node of the scene to be rendered.
        The root node may be used to enumerate the contents of the scene to be rendered, but such an an enumeration has to carefully consider each
        node's properties, such as <i>renderable</i> and <i>hidden</i>, as these properties may imply that the node should be included or excluded
        from the render.
        \remark Consider retrieving the set of nodes using GetSceneNodes(). */
    virtual INode* GetRootNode() const = 0;

    /*! Returns the set of geometric nodes to be rendered.
        This method abstracts the logic necessary to enumerate the scene nodes correctly. It takes into account the standard set of node properties - such
        as <i>hidden</i>, <i>renderable</i>, and <i>selected</i> - along with the render settings to exclude all nodes that should not be rendered.
        The standard behaviour is:
            \li Geometric and light nodes are included if flagged as <i>renderable</i>.
            \li Geometric objects are included if not hidden, unless <i>render hidden objects</i> has been enabled by the user.
            \li If <i>render selected</i> is being used, then geometric objects are included only if selected.
            \li Helper nodes are always all included.
        \param t The time at which to evaluate the nodes.
        \param validity [in, out] The validity of the evaluated nodes is intersected into the input value of this parameter. */
    virtual std::vector<INode*> GetGeometricNodes(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the set of geometric nodes to be rendered.
        This method abstracts the logic necessary to enumerate the scene nodes correctly. It takes into account the standard set of node properties - such
        as <i>hidden</i>, <i>renderable</i>, and <i>selected</i> - along with the render settings to exclude all nodes that should not be rendered.
        The standard behaviour is:
            \li Geometric and light nodes are included if flagged as <i>renderable</i>.
            \li Geometric objects are included if not hidden, unless <i>render hidden objects</i> has been enabled by the user.
            \li If <i>render selected</i> is being used, then geometric objects are included only if selected.
            \li Helper nodes are always all included.
        \param t The time at which to evaluate the nodes.
        \param validity [in, out] The validity of the evaluated nodes is intersected into the input value of this parameter. */
    virtual std::vector<INode*> GetLightNodes(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the set of geometric nodes to be rendered.
        This method abstracts the logic necessary to enumerate the scene nodes correctly. It takes into account the standard set of node properties - such
        as <i>hidden</i>, <i>renderable</i>, and <i>selected</i> - along with the render settings to exclude all nodes that should not be rendered.
        The standard behaviour is:
            \li Geometric and light nodes are included if flagged as <i>renderable</i>.
            \li Geometric objects are included if not hidden, unless <i>render hidden objects</i> has been enabled by the user.
            \li If <i>render selected</i> is being used, then geometric objects are included only if selected.
            \li Helper nodes are always all included.
        \param t The time at which to evaluate the nodes.
        \param validity [in, out] The validity of the evaluated nodes is intersected into the input value of this parameter. */
    virtual std::vector<INode*> GetHelperNodes(const TimeValue t, Interval& validity) const = 0;
    
    /*! Returns the bounding box of all geometric and light nodes to be rendered.
        This method returns a cached (pre-compuated) bounding box that includes all geometric and lights nodes returned by GetSceneNodes(). This
        is useful, for example, to position orthographic cameras and directional lights outside of the scene.
        \param t The time at which to evaluate the nodes.
        \param validity [in, out] The validity of the evaluated nodes is intersected into the input value of this parameter. */
    virtual Box3 GetSceneBoundingBox(const TimeValue t, Interval& validity) const = 0;

    //! \name Notification
    //@{
    //! Registers a notifier callback, to be called when interactive changes occur.
    virtual void RegisterChangeNotifier(IChangeNotifier& notifier) const = 0;
    //! Unregisters a notification callback, previously registered with RegisterChangeNotifier().
    virtual void UnregisterChangeNotifier(IChangeNotifier& notifier) const = 0;
    //@}
    
protected:

    //! Protected destructor prevents destruction through this interface.
    virtual ~ISceneContainer() {}
};

//==================================================================================================
// class ISceneContainer::IChangeNotifier
//
/*! Notification callback interface for ISceneContainer.

    When registered with an ISceneContainer, this interface will be called whenever a relevant change occurs.
*/
class ISceneContainer::IChangeNotifier
{
public:
    //! Notifies that the set of nodes, returned by ISceneContainer::GetSceneNodes(), may have changed.
    virtual void NotifySceneNodesChanged() = 0;
    //! Notifies that the bounding box, returned by ISceneContainer::GetSceneBoundingBox(), may have changed.
    virtual void NotifySceneBoundingBoxChanged() = 0;
protected:
    //! Protected destructor prevents deletion through this interface
    virtual ~IChangeNotifier() {}
};

}}	// namespace

