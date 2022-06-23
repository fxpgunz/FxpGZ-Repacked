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

#include "../BaseInterface.h"
#include "../IImageViewer.h"

class INode;
class ViewExp;

/*! \sa  Class InterfaceServer, Class IInteractiveRender , Class InterfaceServer, Class INode\n\n
\par Description:
This class represents the abstract (interface) for an interactive rendering
manager selector to assist in the process of determining the nodes that are
selected by the interactive rendering manager.  */
class IIRenderMgrSelector : public InterfaceServer
{
public:
	// Is the node selected? Default behaviors shades all nodes
	/*! \remarks This method returns whether the specified node is selected.
	By default all nodes are selected and should therefore be shaded.
	\par Parameters:
	<b>INode* pINode</b>\n\n
	The node to test.
	\par Default Implementation:
	<b>{ return TRUE; }</b> */
	virtual BOOL IsSelected(INode* pINode) { UNUSED_PARAM(pINode); return TRUE; }
};

/*! \sa  Class InterfaceServer, Class IInteractiveRender , Class IIRenderMgrSelector , Class ViewExp\n\n
\par Description:
This class represents the abstract (interface) for an interactive rendering
manager.  */
class IIRenderMgr : public InterfaceServer
{
public:
    enum CommandMode
    {
        CMD_MODE_NULL = 0,
        CMD_MODE_DRAW_REGION,
        CMD_MODE_SELECT_OBJECT,
    }; // may later add modes for camera control

       /*! \remarks This method will indicate the viewport has valid data and can
       execute. */
    virtual bool CanExecute() = 0;
    /*! \remarks This method allows you to activate and deactivate the current
    interactive rendering manager.
    \par Parameters:
    <b>bool active</b>\n\n
    TRUE to enable; FALSE to disable. */
    virtual void SetActive(bool active) = 0;
    /*! \remarks This method will return the name of the render manager. */
    virtual MCHAR* GetName() = 0;
    /*! \remarks This method returns TRUE if the current interactive rendering
    manager is active, otherwise FALSE. */
    virtual bool IsActive() = 0;
    /*! \remarks This method returns a handle to the current window which is
    being rendered to. */
    virtual HWND GetHWnd() const = 0;
    /*! \remarks This method returns a pointer to the ViewExp associated with
    the current interactive rendering manager. */
    virtual ViewExp *GetViewExp() = 0;
    /*! \remarks This method allows you to set the position and size of the
    window being rendered to.
    \par Parameters:
    <b>int X, int Y</b>\n\n
    The x and y screen coordinates of the window.\n\n
    <b>int W, int H</b>\n\n
    The width and height of the window. */
    virtual void SetPos(int X, int Y, int W, int H) = 0;
    /*! \remarks This method will show the window currently being rendered to.
    */
    virtual void Show() = 0;
    /*! \remarks This method will hide the window currently being rendered to.
    */
    virtual void Hide() = 0;
    /*! \remarks This method will issue an update of the current display. */
    virtual void UpdateDisplay() = 0;
    /*! \remarks This method starts the actual rendering process. */
    virtual void Render() = 0;
    /*! \remarks This method allows you to set the delay time in milliseconds.
    */
    virtual void SetDelayTime(int msecDelay) = 0;
    /*! \remarks This method returns the delay time in milliseconds. */
    virtual int GetDelayTime() = 0;
    /*! \remarks This method will close the window currently being rendered
    to. */
    virtual void Close() = 0;
    /*! \remarks This method will delete this render manager. */
    virtual void Delete() = 0;

    // sets and gets the command mode
    /*! \remarks This method allows you to set the command mode.
    \par Parameters:
    <b>CommandMode commandMode</b>\n\n
    One of the following; <b>CMD_MODE_DRAW_REGION</b>, or
    <b>CMD_MODE_SELECT_OBJECT</b>. */
    virtual void SetCommandMode(CommandMode commandMode) = 0;
    /*! \remarks This method returns the command mode, which is one of the
    following; <b>CMD_MODE_DRAW_REGION</b>, or <b>CMD_MODE_SELECT_OBJECT</b>.
    */
    virtual CommandMode GetCommandMode() const = 0;

    // sets and gets the update state
    /*! \remarks This method allows you to define whether an interactive
    rendering action and update should be issued when the mouse button is
    released upward.
    \par Parameters:
    <b>bool actOnlyOnMouseUp</b>\n\n
    TRUE to act only on mouse-up, otherwise FALSE.\n\n
    */
    virtual void SetActOnlyOnMouseUp(bool actOnlyOnMouseUp) = 0;
    /*! \remarks This method returns TRUE if the interactive rendering action
    and update should be issued when the mouse button is released upward,
    otherwise FALSE. */
    virtual bool GetActOnlyOnMouseUp() const = 0;

    // toggles the toolbar display mode (for docked windows)
    /*! \remarks This method toggles the toolbar display mode (for docked
    windows). */
    virtual void ToggleToolbar() const = 0;

    // gets the display style
    /*! \remarks This method returns the display style of the window being
    rendered in, which is one of the following; <b>IV_FLOATING</b> or
    <b>IV_DOCKED</b>. */
    virtual IImageViewer::DisplayStyle GetDisplayStyle() const = 0;

    // find out if the renderer is currently rendering
    /*! \remarks This method returns TRUE if the renderer is currently
    rendering, otherwise FALSE. */
    virtual BOOL IsRendering() = 0;

    // find out if there are update events pending
    /*! \remarks This method returns TRUE if the renderer has not applied
    all pending scene updates yet, or FALSE if there are no updates pending and what
    is currently rendering (or has been rendered) is matching what the scene looks like.\n\n */
    virtual BOOL AnyUpdatesPending() = 0;

    // Has the rendering manager selected any nodes.
    /*! \remarks This method returns TRUE if the rendering manager has any
    selected notes or FALSE if there are none. */
    virtual BOOL AreAnyNodesSelected() const = 0;

    // Get interface that determines whether nodes are selected
    /*! \remarks This method allows you to get the interface that determines
    whether nodes are selected. */
    virtual IIRenderMgrSelector* GetNodeSelector() = 0;

    // ---------------------
    // static public methods
    // ---------------------
    /*! \remarks Obsolete Function */
    static IIRenderMgr* GetActiveIIRenderMgr();
    /*! \remarks Obsolete Function */
    static unsigned int GetNumIIRenderMgrs();
    /*! \remarks Obsolete Function */
    static IIRenderMgr* GetIIRenderMgr(unsigned int i);
};
