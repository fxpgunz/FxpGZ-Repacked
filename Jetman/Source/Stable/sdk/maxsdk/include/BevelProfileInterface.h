#pragma once
//**************************************************************************/
// Copyright (c) 2015 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: Bevel Profile Interface
// AUTHOR: Tom Hudson
//***************************************************************************/

#include "ref.h"

class BevelProfileCurveInterface;
class BevelProfileCurveUser;
class BevelProfileEditorUser;
class PolyLine;

// Originally defined for TextPlus tool; expanded for general use
#define TEXT_BEVEL_PROFILE_CURVE_CLASS_ID Class_ID(0x3f5337aa, 0x2e8e7699)

// This class is used to provide support operations for bevel profiles
#define BEVEL_PROFILE_UTILITY_INTERFACE_CLASS_ID Class_ID(0x7f520b41, 0x41493d26)

// Special notification messages for Bevel Profile Editor windows
#define WM_BEVELPROFILE_TEXT_CHANGE (WM_USER + 0x2000)
#define WM_BEVELPROFILE_REDRAW (WM_USER + 0x2001)
#define WM_BEVELPROFILE_UPDATE_PRESET (WM_USER + 0x2002)
#define WM_BEVELPROFILE_EDITOR_CLOSED (WM_USER + 0x2003)

/**
 * This class provides information on the Bevel Profile Editor to plugins which want to use the standard editor (initially implemented by TextPlus).
 * If you wish to use the Bevel Profile Editor, call BevelProfileInterface::GetBevelProfileEditorInformation (see below), which will return a
 * pointer to this object, which you can use to control the editor.
 * 
 * All methods are implemented by the system.
 */

class BevelProfileEditorInformation
{
public:
	/** Returns the window handle of the Bevel Profile Editor
		\return  The window handle of the Bevel Profile Editor
	*/
	virtual HWND GetHWND()=0;
	/** Get the zoom factor of the Bevel Profile Editor 
		\param z [out] - The zoom factor of the window if the return value is true
		\return  true if the zoom factor is valid for use; false if it is not
	*/
	virtual bool GetZoom(float &z)=0;
	/** Get the horizontal and vertical scroll values of the Bevel Profile Editor
		\param hs [out] - The horizontal scroll value
		\param vs [out] - The vertical scroll value
		\return  true if the scroll values are valid for use; false if they are not
	*/
	virtual bool GetScrolls(int &hs, int &vs)=0;
	/** Sets the zoom factor of the curve window
		\param z - The zoom factor
	*/
	virtual void SetZoom(float z)=0;
	/** Get the horizontal and vertical scroll values of the Bevel Profile Editor
		\param hs - The horizontal scroll value
		\param vs - The vertical scroll value
	*/
	virtual void SetScrolls(int hs, int vs)=0;
	/** Send a message to the Bevel Profile Editor window.
		\param message - The message to send
		\param wParam - Additional message-specific parameter
		\param lParam - Additional message-specific parameter
	*/
	virtual void Notify(UINT message, WPARAM wParam, LPARAM lParam)=0;
	/** Tells the Bevel Profile Editor to display or hide its window
		\param owningWindow - The handle of the window that owns the editor.  This is usually the plugin's parameter rollup window.
		\param sw - If true, displays the editor window.  If false, hides it.
	*/
	virtual void ShowWindow(HWND owningWindow, bool sw = true)=0;
	/** Tells the Bevel Profile Editor window to close
	*/
	virtual void CloseWindow()=0;
	/** Deletes the BevelProfileEditorInformation object.
	*/
	virtual void DeleteMe()=0;
};

/**
 * This object is a system interface which allows plugins to work with Bevel Profiles.
 * When a plugin wants to work with a bevel profile editor or bevel profiles in general, this interface provides several useful functions.
 * To get this interface, call the CreateBevelProfileUtilityInterface function:
 *
 * \code
 *
 *	// Get a Bevel Profile Utility Interface
 *	BevelProfileUtilityInterface* bevelProfileUtilityInterface = CreateBevelProfileUtilityInterface();
 *
 * \endcode
 *
 * All methods are implemented by the system.
 *
 */

class BevelProfileUtilityInterface : public InterfaceServer
{
public:
	/** Get an Interface of the specified type.  Provided for future expansion.
		\param id - The desired interface ID 
		\return  Returns an interface pointer of the specified type if supported, otherwise returns nullptr.
	*/
	virtual BaseInterface* GetInterface(Interface_ID id) override { UNREFERENCED_PARAMETER(id); return nullptr; }
	/** Utility function to load a specified ComboBox control with the available bevel profile preset names
		\param control - The window handle of the ComboBox control that will be loaded with the bevel profile preset names
		\param includeCustom - If true, includes the "(Custom)" bevel profile name
	*/
	virtual void LoadBevelProfilePresetList(HWND control, bool includeCustom = false)=0;
	/** Readies a Bevel Profile Editor for use.  Plugins which want to use the standard Bevel Profile Editor should call this method and
	*   store the returned pointer so that they can control the editor.
		\param user - A pointer to a BevelProfileCurveUser object (i.e. the plugin which is using the editor)
		\return  A pointer to the BevelProfileEditorInformation (see above)
	*/
	virtual BevelProfileEditorInformation* GetBevelProfileEditorInformation(BevelProfileCurveUser* user)=0;
	/** Delete this interface.  When you are through using this interface, it should be destroyed using this method.
	*/
	virtual void DeleteMe() { delete this; }
};

/**
 * To get a BevelProfileUtilityInterface object (see above), call this core function.  It will return a pointer to the interface, which 
 * you can then use to perform functions related to bevel profile curve editing.
 *
 * An example of the use of the bevel profile curves may be found in the Bevel Profile modifier.
		\return  A BevelProfileCurveInterface object containing the bevel profile curve
 */
CoreExport BevelProfileUtilityInterface* CreateBevelProfileUtilityInterface();

/**
 * A plugin which wants to use the Bevel Profile Curves should derive from this class and implement the following methods.
 */

class BevelProfileCurveUser
{
public:
	/** Lets the curve user know that the bevel profile has changed in some way.  This can be via the user selecting a different profile or other
	*   editing methods if the Bevel Profile Editor is in use.  The plugin should update its model if it has not been inhibited via the InhibitMesh call (see below)
	*/
	virtual void ChangedBevelProfile()=0;
	/** Tells the plugin to update or not update its mesh on changes to the profile.  The plugin should maintain an internal switch to keep track of this setting.
		\param sw - If true, the plugin should ignore changes to the bevel profile curve untin this method is called again with 'sw' set to false.
	*/
	virtual void InhibitMesh(bool sw)=0;
	/** When this is called, if the plugin is using the Bevel Profile Editor, it should call bevelProfileEditorInfo->Notify(WM_BEVELPROFILE_REDRAW, 0, 0) to force a redraw
	*/
	virtual void RedrawBevelProfile()=0;
	/** When this is called, the plugin should return its auto bevel update switch state.  This lets the editor call beck to tell the plugin to update itrs mesh when
	*   changes to the curve are made.
		\return  true if the mesh should auto-update on changes to the bevel profile curve
	*/
	virtual bool GetAutoBevelUpdate()=0;
	/** When this is called, the plugin should store the switch state for use when GetAutoBevelUpdate is called.
		\param sw - The automatic update flag.
	*/
	virtual void SetAutoBevelUpdate(bool sw)=0;
	/** Get the bevel profile editor window handle
		\return  Plugins should return bevelProfileEditorInfo->GetHWND() (see above)
	*/
	virtual HWND GetBevelProfileEditorHWND()=0;
	/** The system will call this method to set the plugin's bevel profile curve 
		\param curve - The bevel profile curve to set.  This should be stored as a reference in your plugin
	*/
	virtual void SetBevelProfileCurveInterface(BevelProfileCurveInterface* curve)=0;
	/** When this is called, the plugin should return its bevel profile curve interface.
		\return  The BevelProfileCurveInterface in use by the plugin
	*/
	virtual BevelProfileCurveInterface* GetBevelProfileCurveInterface()=0;
	/** This is called when the bevel peset selection has changed.  When this call is received, the plugin should update its current bevel preset selection
	*   in the UI by calling BevelProfileInterface::LoadBevelProfilePresetList(presetList, true);
	*/
	virtual void BevelPresetSelectionChanged()=0;

	// If using the Bevel Profile Editor, the following are required

	/** When called, the plugin should return its BevelProfileEditorInformation pointer.
		\return  The plugin's BevelProfileEditorInformation pointer
	*/
	virtual BevelProfileEditorInformation* GetBevelProfileEditorInformation()=0;
	/** When called, the plugin should return the handle of the window that owns the Bevel Profile Editor.
		\return  The handle of the window which owns the editor.  Typically this will be the plugin's main rollup window.
	*/
	virtual HWND BevelProfileEditorOwningWindow()=0;
};

/**
 * This is the interface to the actual bevel profile curve.  It contains methods necessary to select presets, get validity intervals,
 * and get a PolyLine that is scaled for use.
 * 
 * All methods are implemented by the system.
 */

class BevelProfileCurveInterface : public ReferenceTarget
{
public:
	/** Returns a unique identifier for the curve type.  For possible future expansion.  For internal use only.
		\return  A curve type ID used to validate the curve
	*/
	virtual ulong CurveTypeID()=0;
	/** Reset the curve to a straight bevel.
	*/
	virtual void Reset()=0;
	/** Set the BevelProfileCurveUser for this profile curve.
		\param u - The BevelProfileCurveUser
		\return  Info
	*/
	virtual void SetBevelProfileUser(BevelProfileCurveUser *u)=0;
	/** Instructs the curve to dispose of any cached data
	*/
	virtual void InvalidateCache()=0;
	/** Tells the curve to load a preset of a given name.  This should be a name in a ComboBox control that you have previously populated via use of the
	*   BevelProfileInterface::LoadBevelProfilePresetList method (see above)
		\param name - The name of the preset curve to load.
	*/
	virtual void LoadPresetCurve(const MSTR& name)=0;
	/** Tells the bevel profile curve to change the selected name in its preset list to the specified name.
		\param presetList - The window handle of a ComboBox control containing preset curve names
	*/
	virtual void SelectProfilePresetName(HWND presetList)=0;
	/** Tells the curve to select its current preset name from the list in a ComboBox indicated by the supplied window handle.  This should be a control that you
	*   have previously populated via use of the BevelProfileInterface::LoadBevelProfilePresetList method (see above)
		\param u - The BevelProfileCurveUser (the plugin)
		\param presetName - The preset name to switch to
	*/
	virtual void ChangeToCurve(BevelProfileCurveUser *u, const MSTR& presetName)=0;
	/** Tells the BevelProfileCurve to copy its data from another curve.
		\param from - The curve to copy from
	*/
	virtual void CopyFrom(BevelProfileCurveInterface* from)=0;
	/** Instructs the bevel profile curve to create PolyLine that is scaled to the specified values.
		\param t - The time at which to sample the curve
		\param bevelWidth - The width in world units to fit the curve (X)
		\param bevelDepth - The height in world units to fit the curve (Y)
		\param bevelPush - The amount to push the curve 
		\param bevelSteps - The number of interpolation steps
		\param bevelOptimize - When true, straight curve segments will use 0 steps instead of bevelSteps
		\param bevelProfile [out] - The PolyLine object that will receive the curve
	*/
	virtual void MakePolyline(TimeValue t, float bevelWidth, float bevelDepth, float bevelPush, int bevelSteps, bool bevelOptimize, PolyLine &bevelProfile)=0;
	/** Returns the validity interval of the curve (it may be animated)
		\param t - The time at which to sample the validity interval
		\return  The validity interval at the specified time
	*/
	virtual Interval GetValidity(TimeValue t)=0;
	/** Query the curve to see if it is a custom curve or an unmodified preset
		\return  true if the curve is custom; false if it is an unmodified preset.
	*/
	virtual bool IsCustom()=0;
};

/**
 * To use 3ds Max's standard bevel profile curves, make your plugin class a subclass of BevelProfileCurveUser, 
 * then call this function, providing a pointer to your object as the 'user' parameter.  The system will create
 * and return a pointer to a standard bevel profile curve, which you should then make a reference to, as these curves can be animated.
 *
 * An example of the use of the bevel profile curves may be found in the Bevel Profile modifier.
		\param user - Pointer to your BevelProfileUser object (your plugin)
		\return  A BevelProfileCurveInterface object containing the bevel profile curve
 */
CoreExport BevelProfileCurveInterface* CreateStandardBevelProfileCurve(BevelProfileCurveUser* user);
