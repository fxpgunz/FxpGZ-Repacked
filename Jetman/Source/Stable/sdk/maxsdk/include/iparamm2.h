/**********************************************************************
 *<
	FILE: IParamM2.h

	DESCRIPTION:  Parameter Maps, Edition 2 for use with ParamBlock2's

	CREATED BY: Rolf Berteig
			    John Wainwright, 2nd edition

	HISTORY: created 10/10/95
			 2nd Ed. 9/8/98

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

/*
 *   IParamMap2's are used to map and manage UI dialogs for the parameters
 *   in a ParamBlock2.  They work almost identically to IParamMaps except:
 *
 *    1. they only work with ParamBlock2-hosted parameters; there is no
 *       IParamArray equivalent.  Hopefully, the new capabilities of ParamBlock2's
 *       cover most of the reasons for using IParamArrays.  If not, install virtual
 *       paramters in the ParamBlock2 and supply accessor fns for them.
 *
 *	  2. they derive all UI-related metadata from the ParamBlockDesc2 structure now
 *       pointed to by a ParamBlock2; there is no ParamUIDesc equivalent.
 *
 *    3. some new methods on ClassDesc can be used to automatically construct & open
 *       rollouts, so you may not have to create these explicitly yourself.
 */

#pragma once

#include "maxheap.h"
#include "iparamb2.h"
#include "render.h"
#include "pbbitmap.h"

// forward declarations
class IParamMap2;
class IRendParams;
class SpecialFX;

// If custom handling of controls needs to be done, ParameterMap
// client can't implement one of these and set is as the ParameterMap's
// user callback.
/*! \sa  Class ParamBlockDesc2, Class ClassDesc2.\n\n
\par Description:
If there are
controls which require custom handling you can create an object from this class
and set it as the parameter map's user callback (usually using
<b>SetUserDialogProc()</b>).  */
#pragma warning(push)
#pragma warning(disable:4100)
class ParamMap2UserDlgProc: public MaxHeapOperators 
{
	public:
		/*! \remarks Destructor */
		virtual ~ParamMap2UserDlgProc() { }
		virtual INT_PTR DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)=0;
		/*! \remarks This method is called to delete this object. If the DlgProc is non-NULL
		when the ParamMap is deleted this method will be called. */
		virtual void DeleteThis()=0;
		/*! \remarks This gets called if the <b>DlgProc</b> is registered with a parameter
		map associated with one of the <b>IAutoXXXParamDlg</b> instances and
		that dialog has <b>SetThing()</b> called on it. This provides a simple
		way for the user dialog proc to track changes to the underlying object
		in the parameter map.
		\par Parameters:
		<b>ReferenceTarget *m</b>\n\n
		The item that was set.
		\par Default Implementation:
		<b>{}</b> */
		virtual void SetThing(ReferenceTarget *m) { }
		virtual void Update(TimeValue t) { }
		virtual void SetParamBlock(IParamBlock2 *pb) { }
		// alternative update add to R4, for extra context & influence over pmap validity interval - jbw 9.9.0
		/*! \remarks This is a variant of <b>Update</b>() method in
		<b>ParamMap2UserDlgProc</b> used to supply more context, including the
		pmap itself and its validity interval. Changes you make to the validity
		internal passed in affect the validity interval of the whole parammap.
		\par Parameters:
		<b>TimeValue t</b>\n\n
		The time at which the update is taking place.\n\n
		<b>Interval\& valid</b>\n\n
		The validity interval of the parameter map.\n\n
		<b>IParamMap2 *pmap</b>\n\n
		The parameter map the user dialog proc is associated with. */
		virtual void Update(TimeValue t, Interval& valid, IParamMap2* pmap) { this->Update(t); }
};
#pragma warning(pop)

/*! \sa  Class IParamBlock2, Class ParamBlockDesc2, Class ParamMap2UserDlgProc.\n\n
\par Description:
This class provides methods to work with parameter map2s. Methods are provided
for things like invalidating the parameter map so it gets redrawn, working with
the parameter blocks associated with the parameter map, and establishing an
optional dialog proc to handle controls not directly handled by the parameter
map. This section also documents several functions that are available for
creating and destroying parameter maps but are not part of this class.\n\n
Note: The use of this class requires the explicit inclusion the IPARAMM2.H
header file.  */
class IParamMap2 : public InterfaceServer
{
	public:
	 	// UI updating.
	 	/*! \remarks This method marks the user interface as needing to be
	 	updated. This affects the entire UI for the parameter map. */
	 	virtual void Invalidate()=0;    // whole UI
	 	/*! \remarks This methods un-invalidates the entire user interface.
	 	*/
	 	virtual void Validate()=0;		// uninvalidate whole UI
		/*! \remarks This methods marks a specific control in the UI as
		requiring an update.
		\param id The permanent ID of the control requiring an update.
		\param tabIndex If the control is a Tab\<\> then this is the zero based index into the
		table of the value to be invalidated. */
		virtual void Invalidate(ParamID id, int tabIndex=0)=0;  // nominated param
		/*! \remarks This method will update the user interface if the current
		settings are not valid at the given time (i.e., if anything is animated
		at that time).
		\param t The time to check for the update. */
		virtual void UpdateUI(TimeValue t)=0;  // update UI directly for time t
		/*! \remarks This method may be called to cause the viewports to be
		redrawn.
		\param t The time at which to redraw the viewports.
		\param flag You may specify one of the following:\n\n
		<b>REDRAW_BEGIN</b>\n
		Call this before you redraw.\n\n
		<b>REDRAW_INTERACTIVE</b>\n
		This allows the view quality to degrade to maintain interactively.\n\n
		<b>REDRAW_END</b>\n
		If during interactive redraw the state degraded, this will redraw the
		views in the undegraded state.\n\n
		<b>REDRAW_NORMAL</b>\n
		This redraws the views in the undegraded state. */
		virtual void RedrawViews(TimeValue t, DWORD flag=REDRAW_NORMAL)=0;  // redraw viewport
		/*! \remarks Provides access to pmap's validity interval. Returns a refernce to the
		actual interval so it can be modified if desired. */
		virtual Interval& Validity()=0;	// access to map's validity interval

		/*! Makes the parameter map point to a new parameter block, updating the user interface in the process.
            \remark Consider calling SetParamBlock(IParamBlock2 *pb, ReferenceMaker* owner) instead.
            \param pb The new parameter block to use with the parameter map.
            \remarks This method swaps the existing parameter block with a new */
		virtual void SetParamBlock(IParamBlock2 *pb) = 0;
		/*! Makes the parameter map point to a new parameter block, updating the user interface in the process.
            \param pb The new parameter block to use with the parameter map.
            \param owner The owner the parameter block. In most cases, this should be IParamBlock2::GetOwner(). But in the case 
                of class parameter block (see P_CLASS_PARAMS), where the parameter block does not have an owner, then this should
                point to the object for which the UI is currently being edited. */
        virtual void SetParamBlock(IParamBlock2 *pb, ReferenceMaker* owner) = 0;
		/*! \remarks Returns the parameter map's ID. */
		virtual MapID GetMapID()=0;

		// The given proc will be called _after_ default processing is done.
		// The callback can then apply constraints to controls.
		// Note that if the proc is non-NULL when the ParamMap is deleted
		// its DeleteThis() method will be called.
		/*! \remarks This method allows a developer to provide special
		handling for one or more controls. The developer provides a dialog proc
		to process the message from the control. This method is used to tell
		the parameter map that the developer defined method should be called.
		The given proc will be called after default processing is done. Note
		that if the proc is non-NULL when the ParamMap2 is deleted its
		DeleteThis() method will be called.
		\param proc Points to the user dialog proc object to process the controls. */
		virtual void SetUserDlgProc(ParamMap2UserDlgProc *proc=NULL)=0;
		/*! \remarks Returns a pointer to the user dialog proc for the
		parameter map (or NULL if none is defined). See <b>SetUserDlgProc()</b>
		above. */
		virtual ParamMap2UserDlgProc *GetUserDlgProc()=0;

		// Changes a map entry to refer to a different item in the parameter block.
		/*! \remarks This method changes a parameter map entry to refer to a
		different item in the parameter block.
		\param curParam The parameter ID of the item to change.
		\param newParam The new parameter ID. */
		virtual void ReplaceParam(ParamID curParam, ParamID newParam) { UNUSED_PARAM(curParam); UNUSED_PARAM(newParam); }

		// Access the dialog window.
		/*! \remarks Returns the dialog window handle of the parameter map. */
		virtual HWND GetHWnd()=0;
        //! \brief Returns the Qt widget being controlled by this parameter map, if applicable.
        //! This method will return a valid Qt widget only if the parameter map was constructed from a Qt widget. It will return null if the 
        //! parameter map controls a Win32 dialog.
        virtual MaxSDK::QMaxParamBlockWidget* GetQWidget() const = 0;
		// Access the rollup window containing this rollout dialog
		virtual IRollupWindow* GetIRollup() { return NULL; }

		// Access the parameter block
		/*! \remarks Returns a pointer to the parameter block2 used by this
		parameter map. */
		virtual IParamBlock2 *GetParamBlock()=0;

		// Is the dialog proc active
		/*! \remarks Returns TRUE if the parameter map dialog proc is active;
		otherwise FALSE. */
		virtual BOOL DlgActive()=0;

		// Access my descriptor
		/*! \remarks Returns a pointer to the ParamBlockDesc2 instance for
		this parameter map. */
		virtual ParamBlockDesc2* GetDesc()=0;

		// sent to indicate dialog is going inactive so, among other things, ColorSwatches can be told
		/*! \remarks This method is called to indicate the dialog is going
		inactive or is becoming active.
		\param onOff TRUE if becoming active; FALSE for inactive. */
		virtual void ActivateDlg(BOOL onOff)=0;

		// sent to a Material Editor map to find the SubTex or SubMtl index corresponding to the control hw	
		/*! \remarks This method is sent to a Material Editor map to find the
		SubTex index corresponding to the control handle. It should return the
		index of the sub-texmap corresponding to the window whose handle is
		passed. If the handle is not valid return -1.
		\param hw The window handle of the control. */
		virtual int FindSubTexFromHWND(HWND hw)=0;
		virtual int FindSubMtlFromHWND(HWND hw)=0;

		// Individual enable of param UI controls
		/*! \remarks This method is used to enable of disable an individual
		user interface control.
		\param id The parameter ID of the control to enable/disable.
		\param onOff TRUE to enable; FALSE to disable.
		\param tabIndex If the control is a Tab\<\> then this is the zero based index in the
		table of the item to enable/disable. */
		virtual void Enable(ParamID id, BOOL onOff, int tabIndex=0)=0;

		// Set text of param UI control
		/*! \remarks This method sets the text of a parameter user interface
		control.
		\param id The ID of the parameter whose user interface text to change.
		\param txt The new string to display.
		\param tabIndex If the parmeter is a Tab\<\> this is the zero based index of the
		parameter in the table. */
		virtual void SetText(ParamID id, const MCHAR* txt, int tabIndex=0)=0;

		//! \brief Set tooltip of param UI control
		/*! This method allows for turning on or off the tooltip for a parameter user interface control.
		\param id - The ID of the parameter whose user interface tooltip to set.
		\param bEnable - if true, the tooltip is enabled, otherwise disabled. 
		When disabled, the tooltip won't be displayed when the the mouse hovers on top of the control.
		Enabling the tooltip repeatedly will have the same effect as enabling it once.
		\param txt - pointer to a string representing the text to be displayed in the tooltip window.
		This parameter is ignored when the tooltip is being disabled.
		\param tabIndex If the parameter is a Tab\<\> this is the zero based index of the
		parameter in the table. */
		virtual void SetTooltip(ParamID id, bool bEnable, const MCHAR* txt, int tabIndex=0)=0;

		// Set range of param UI control (spinner/slider)
		/*! \remarks This method sets the range of parameter for a spinner or
		slider control.
		\param id The ID of the parameter.
		\param low The low range for the spinner / slider.
		\param high The high range for the spinner / slider.
		\param tabIndex If the parmeter is a Tab\<\> this is the zero based index of the
		parameter in the table. */
		virtual void SetRange(ParamID id, float low, float high, int tabIndex=0)=0;

		// show or hide assciated controls
		/*! \remarks This method will show or hide the specified control.
		\param id The ID of the parameter.
		\param showHide TRUE to show; FALSE to hide.
		\param tabIndex If the parmeter is a Tab<> this is the zero based index of the
		parameter in the table. */
		virtual void Show(ParamID id, BOOL showHide, int tabIndex=0)=0;

		// sent by any AutoXXParamDlg as a courtesy when it receives a SetThing()
		/*! \remarks This method is called by any <b>IAutoXXParamDlg</b> when
		it receives a <b>SetThing()</b>.
		\param m The item which was set. */
		virtual void SetThing(ReferenceTarget *m)=0;
};

// Creates a parameter map that will handle a parameter block in a modeless
// dialog where time does not change and the viewport is not redrawn.
// Note that there is no need to destroy it. It executes the dialog and then
// destorys itself. Returns TRUE if the user selected OK, FALSE otherwise.
/*! \remarks Creates a parameter map that will handle a parameter block in a modeless dialog
where time does not change and the viewports are not redrawn. This overload of
<b>CreateModalParamMap2()</b> has a new parameter, <b>map_id</b>, that
specifies the ID of the parameter map/rollup to be created for this particular
parameter block. See original function for the rest of the description. */
PB2Export BOOL CreateModalParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		TimeValue t,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		HWND hParent,
		ParamMap2UserDlgProc *proc=NULL);
/*! \remarks This function creates a parameter map that will handle a
parameter block in a modeless dialog where time does not change and the
viewports are not redrawn. Note that there is no need to destroy it. It
executes the dialog and then destorys itself.\n\n
Note, in version 4.0 and later, this actually maps to a call on the explicit
map ID overload of <b>CreateModalParamMap2()</b> with default map ID of 0.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>TimeValue t</b>\n\n
The time at which the dialog is launched.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
The dialog template.\n\n
<b>HWND hParent</b>\n\n
The parent window.\n\n
<b>ParamMap2UserDlgProc *proc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.
\return  Returns TRUE if the user selected OK, FALSE otherwise. */
inline BOOL CreateModalParamMap2(
		IParamBlock2 *pb,
		TimeValue t,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		HWND hParent,
		ParamMap2UserDlgProc *proc=NULL) { return CreateModalParamMap2(0, pb, t, hInst, dlgTemplate, hParent, proc); } 

PB2Export BOOL CreateModalParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		TimeValue t,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		HWND hParent,
		ParamMap2UserDlgProc *proc=NULL);
/*! \remarks This function creates a parameter map that will handle a parameter block in a
modeless dialog where time does not change and the viewports are not redrawn.
Note that there is no need to destroy it. It executes the dialog and then
destorys itself. This function is currently not in use.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>TimeValue t</b>\n\n
The time at which the dialog is launched.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>DLGTEMPLATE *dlgTemplate</b>\n\n
The dialog template.\n\n
<b>HWND hParent</b>\n\n
The parent window.\n\n
<b>ParamMap2UserDlgProc *proc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.
\return  Returns TRUE if the user selected OK, FALSE otherwise. */
inline BOOL CreateModalParamMap2(
		IParamBlock2 *pb,
		TimeValue t,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		HWND hParent,
		ParamMap2UserDlgProc *proc=NULL) { return CreateModalParamMap2(0, pb, t, hInst, dlgTemplate, hParent, proc); } 

// create a child dialog of the given parent parammap (for tabbed dialogs, etc.)
PB2Export IParamMap2* CreateChildModalParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		HINSTANCE hInst,
		IParamMap2* parent,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc *proc=NULL);
inline IParamMap2* CreateChildModalParamMap2(
		IParamBlock2 *pb,
		HINSTANCE hInst,
		IParamMap2* parent,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc *proc=NULL) { return CreateChildModalParamMap2(0, pb, hInst, parent, dlgTemplate, title, proc); }

#ifndef NO_MODELESS_PARAMMAP
PB2Export IParamMap2 *CreateModelessParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		TimeValue t,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		HWND hParent,
		ParamMap2UserDlgProc *proc=NULL);
inline IParamMap2 *CreateModelessParamMap2(
		IParamBlock2 *pb,
		TimeValue t,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		HWND hParent,
		ParamMap2UserDlgProc *proc=NULL) { return CreateModelessParamMap2(0, pb, t, hInst, dlgTemplate, hParent, proc); } 

PB2Export IParamMap2 *CreateModelessParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		TimeValue t,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		HWND hParent,
		ParamMap2UserDlgProc *proc=NULL);
inline IParamMap2 *CreateModelessParamMap2(
		IParamBlock2 *pb,
		TimeValue t,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		HWND hParent,
		ParamMap2UserDlgProc *proc=NULL) { return CreateModelessParamMap2(0, pb, t, hInst, dlgTemplate, hParent, proc); } 

PB2Export void DestroyModelessParamMap2(IParamMap2* m);
#endif //NO_MODELESS_PARAMMAP

PB2Export IParamMap2* CreateChildModalParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		HINSTANCE hInst,
		IParamMap2* parent,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc *proc=NULL);
inline IParamMap2* CreateChildModalParamMap2(
		IParamBlock2 *pb,
		HINSTANCE hInst,
		IParamMap2* parent,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc *proc=NULL) { return CreateChildModalParamMap2(0, pb, hInst, parent, dlgTemplate, title, proc); }

/*! \remarks This function destroys a parameter map created by
<b>CreateChildModalParamMap2()</b>.
\par Parameters:
<b>IParamMap2 *m</b>\n\n
Points to the parameter map2 to destroy. */
PB2Export void DestroyChildModalParamMap2(IParamMap2 *m);

// Creates a parameter map to handle the display of parameters in the command panal.
// This will add a rollup page to the command panel.
// DestroyCPParamMap().
/*! \remarks Creates a parameter map to handle the display of parameters in the command
panel. This overload of <b>CreateCPParamMap2()</b> has a new parameter,
<b>map_id</b>, that specifies the ID of the parameter map/rollup to be created
for this particular parameter block. See original function for the rest of the
description. */
PB2Export IParamMap2 *CreateCPParamMap2(
		MapID map_id,
		IParamBlock2 *pb,
		Interface *ip,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		HWND hOldRollup=NULL,
		int category = ROLLUP_CAT_STANDARD );
/*! \remarks This function creates a parameter map to handle the display of
parameters in the command panel. This will add a rollup page to the command
panel (or optionally replace an existing one).\n\n
Note, in version 4.0 and later, this actually maps to a call on the explicit
map ID overload of <b>CreateCPParamMap2()</b> with default map ID of 0.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>Interface *ip</b>\n\n
Pass in the plug-ins interface pointer.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
The dialog template.\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the rollup page title bar.\n\n
<b>DWORD flags</b>\n\n
A flag to control the settings of the rollup page:\n\n
<b>APPENDROLL_CLOSED</b>\n\n
Starts the page in the rolled up state.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL,</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.\n\n
<b>HWND hOldRollup=NULL</b>\n\n
If non-NULL specifies an existing rollup window in the current UI context that
should be replaced with the newly created rollup for this map.\n\n
<b>int category = ROLLUP_CAT_STANDARD</b>\n\n
The category parameter provides flexibility with regard to where a particular
rollup should be displayed in the UI. RollupPanels with lower category fields
will be displayed before RollupPanels with higher category fields. For
RollupPanels with equal category value the one that was added first will be
displayed first. Allthough it is possible to pass any int value as category
there exist currently 5 different category defines: <b>ROLLUP_CAT_SYSTEM</b>,
<b>ROLLUP_CAT_STANDARD</b>, and <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
category sticky, meaning it will not read the category from the
<b>RollupOrder.cfg</b> file, but rather save the category field that was passed
as argument in the <b>CatRegistry</b> and in the <b>RollupOrder.cfg</b>
file.\n\n
The method will take the category of the replaced rollup in case the flags
argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
this system works with param maps as well.
\return  Returns a pointer to the parameter map2. */
inline IParamMap2 *CreateCPParamMap2(
		IParamBlock2 *pb,
		Interface *ip,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		HWND hOldRollup=NULL,
		int category = ROLLUP_CAT_STANDARD ) { return CreateCPParamMap2(0, pb, ip, hInst, dlgTemplate, title, flags, dlgProc, hOldRollup, category); }

PB2Export IParamMap2 *CreateCPParamMap2(
		MapID map_id,
		IParamBlock2 *pb,
		Interface *ip,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		HWND hOldRollup=NULL,
		int category = ROLLUP_CAT_STANDARD );
/*! \remarks This function creates a parameter map to handle the display of parameters in
the command panel. This will add a rollup page to the command panel (or
optionally replace an existing one). This function is currently not used.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>Interface *ip</b>\n\n
Pass in the plug-ins interface pointer.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
The dialog template.\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the rollup page title bar.\n\n
<b>DWORD flags</b>\n\n
A flag to control the settings of the rollup page:\n\n
<b>APPENDROLL_CLOSED</b>\n\n
Starts the page in the rolled up state.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL,</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.\n\n
<b>HWND hOldRollup=NULL</b>\n\n
If non-NULL specifies an existing rollup window in the current UI context that
should be replaced with the newly created rollup for this map.\n\n
<b>int category = ROLLUP_CAT_STANDARD</b>\n\n
The category parameter provides flexibility with regard to where a particular
rollup should be displayed in the UI. RollupPanels with lower category fields
will be displayed before RollupPanels with higher category fields. For
RollupPanels with equal category value the one that was added first will be
displayed first. Allthough it is possible to pass any int value as category
there exist currently 5 different category defines: <b>ROLLUP_CAT_SYSTEM</b>,
<b>ROLLUP_CAT_STANDARD</b>, and <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
category sticky, meaning it will not read the category from the
<b>RollupOrder.cfg</b> file, but rather save the category field that was passed
as argument in the <b>CatRegistry</b> and in the <b>RollupOrder.cfg</b>
file.\n\n
The method will take the category of the replaced rollup in case the flags
argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
this system works with param maps as well.
\return  Returns a pointer to the parameter map2. */
inline IParamMap2 *CreateCPParamMap2(
		IParamBlock2 *pb,
		Interface *ip,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		HWND hOldRollup=NULL,
		int category = ROLLUP_CAT_STANDARD ) { return CreateCPParamMap2(0, pb, ip, hInst, dlgTemplate, title, flags, dlgProc, hOldRollup, category); }

//! \brief Creates a parameter map which manages a Qt dialog, in the command panel.
/*! For more information on using Qt widgets with parameter maps, see: \ref parammap_qt.
    \param map_id The ID of the parameter map, useful if multiple rollups should be created for the same parameter block. Pass 0 if not creating multiple maps.
    \param pb The param block to be controlled by the parameter map.
    \param ip The interface passed to Animatable::BeginEditParams().
    \param qtWidget The Qt widget which forms the dialog to be controlled. This widget will be inserted into the rollout. The parameter map assumes the ownership of the widget and will delete when it gets destroyed.
    \param title The title of the rollout.
    \param flags Rollout flags, such as APPENDROLL_CLOSED.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The parameter map which controls the relationship between the UI dialog and the parameter block. */
PB2Export IParamMap2 *CreateCPParamMap2(
    MapID map_id,
    IParamBlock2 *pb,
    Interface *ip,
    MaxSDK::QMaxParamBlockWidget& qtWidget,
    const MCHAR *title,
    DWORD flags,
    int category = ROLLUP_CAT_STANDARD );

/*! \remarks This function destroys a parameter map created by
<b>CreateCParamMap2()</b>. The rollup page is removed and the parameter map is
deleted.
\par Parameters:
<b>IParamMap2 *m</b>\n\n
A pointer to the parameter map2 to delete. */
PB2Export void DestroyCPParamMap2(IParamMap2 *m);

// create a child dialog of the given parent parammap (for tabbed dialogs, etc.)
/*! \remarks Create a child dialog of the given parent command panel parammap (for tabbed
dialogs, etc.) This overload of <b>CreateChildCPParamMap2()</b> has a new
parameter, <b>map_id</b>, that specifies the ID of the parameter map/rollup to
be created for this particular parameter block. See original function for the
rest of the description. */
PB2Export IParamMap2 *CreateChildCPParamMap2(
		MapID map_id,
		IParamBlock2 *pb,
		Interface *ip,
		HINSTANCE hInst,
		IParamMap2* parent,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc* dlgProc=NULL);
/*! \remarks This function creates a child dialog of the given parent parammap
(for tabbed dialogs, etc.). This version takes an extra parent IParamMap2* and
creates a child dialog window in the parent parammap's window (rather than a
new rollup) that is mapped by the new parammap. Developers need to call this
explicitly once the parent parammap has been created as child parammaps are not
created automatically by the <b>P_AUTO_UI</b> mechanisms.\n\n
Note, in version 4.0 and later, this actually maps to a call on the explicit
map ID overload of <b>CreateChildCPParamMap2()</b> with default map ID of 0.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>Interface *ip</b>\n\n
Pass in the plug-ins rendering parameters interface pointer.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>IParamMap2* parent</b>\n\n
The parent parameter map.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
The dialog template.\n\n
<b>MCHAR *title</b>\n\n
The title displayed.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.
\return  Returns a pointer to the parameter map2. */
inline IParamMap2 *CreateChildCPParamMap2(
		IParamBlock2 *pb,
		Interface *ip,
		HINSTANCE hInst,
		IParamMap2* parent,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc* dlgProc=NULL) { return CreateChildCPParamMap2(0, pb, ip, hInst, parent, dlgTemplate, title, dlgProc); }

PB2Export IParamMap2 *CreateChildCPParamMap2(
		MapID map_id,
		IParamBlock2 *pb,
		Interface *ip,
		HINSTANCE hInst,
		IParamMap2* parent,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc* dlgProc=NULL);
/*! \remarks This function creates a child dialog of the given parent parammap (for tabbed
dialogs, etc.). This version takes an extra parent IParamMap2* and creates a
child dialog window in the parent parammap's window (rather than a new rollup)
that is mapped by the new parammap. Developers need to call this explicitly
once the parent parammap has been created as child parammaps are not created
automatically by the <b>P_AUTO_UI</b> mechanisms. This function is currently
not used.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>Interface *ip</b>\n\n
Pass in the plug-ins rendering parameters interface pointer.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>IParamMap2* parent</b>\n\n
The parent parameter map.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
The dialog template.\n\n
<b>MCHAR *title</b>\n\n
The title displayed.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.
\return  Returns a pointer to the parameter map2. */
inline IParamMap2 *CreateChildCPParamMap2(
		IParamBlock2 *pb,
		Interface *ip,
		HINSTANCE hInst,
		IParamMap2* parent,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc* dlgProc=NULL) { return CreateChildCPParamMap2(0, pb, ip, hInst, parent, dlgTemplate, title, dlgProc); }

/*! \remarks This function destroys a parameter map created by
<b>CreateChildCPParamMap2()</b>.
\par Parameters:
<b>IParamMap2 *m</b>\n\n
Points to the parameter map2 to destroy. */
PB2Export void DestroyChildCPParamMap2(IParamMap2 *m);

// Creates a parameter map to handle the display of render parameters or
// atmospheric plug-in parameters.
/*! \remarks Creates a parameter map to handle the display of render parameters or
atmospheric plug-in parameters. This overload of <b>CreateRParamMap2()</b> has
a new parameter, <b>map_id</b>, that specifies the ID of the parameter
map/rollup to be created for this particular parameter block. See original
function for the rest of the description. */
PB2Export IParamMap2 *CreateRParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		IRendParams *ip,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		int category = ROLLUP_CAT_STANDARD);
/*! \remarks Creates a parameter map to handle the display of render
parameters or atmospheric plug-in parameters.\n\n
Note, in version 4.0 and later, this actually maps to a call on the explicit
map ID overload of <b>CreateRParamMap2()</b> with default map ID of 0.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>IRendParams *ip</b>\n\n
Pass in the plug-ins rendering parameters interface pointer. See
Class IRendParams.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
The dialog template.\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the rollup page title bar.\n\n
<b>DWORD flags</b>\n\n
A flag to control the settings of the rollup page:\n\n
<b>APPENDROLL_CLOSED</b>\n\n
Starts the page in the rolled up state.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.\n\n
<b>int category = ROLLUP_CAT_STANDARD</b>\n\n
The category parameter provides flexibility with regard to where a particular
rollup should be displayed in the UI. RollupPanels with lower category fields
will be displayed before RollupPanels with higher category fields. For
RollupPanels with equal category value the one that was added first will be
displayed first. Allthough it is possible to pass any int value as category
there exist currently 5 different category defines: <b>ROLLUP_CAT_SYSTEM</b>,
<b>ROLLUP_CAT_STANDARD</b>, and <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
category sticky, meaning it will not read the category from the
<b>RollupOrder.cfg</b> file, but rather save the category field that was passed
as argument in the <b>CatRegistry</b> and in the <b>RollupOrder.cfg</b>
file.\n\n
The method will take the category of the replaced rollup in case the flags
argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
this system works with param maps as well.
\return  Returns a pointer to the parameter map2. */
inline IParamMap2 *CreateRParamMap2(
		IParamBlock2 *pb,
		IRendParams *ip,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		int category = ROLLUP_CAT_STANDARD) { return CreateRParamMap2(0, pb, ip, hInst, dlgTemplate, title, flags, dlgProc, category); }

PB2Export IParamMap2 *CreateRParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		IRendParams *ip,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		int category = ROLLUP_CAT_STANDARD);

/*! \page parammap_qt Qt in Parameter Maps and Parameter Blocks

    Qt dialogs are the preferred way of managing automatic parameter mapping between parameter blocks and their UIs.
    To map a Qt dialog to a parameter block, use the P_AUTO_UI_QT flag in the parameter block descriptor. Individual parameters should <b>not</b>
    specify UI properties (through the p_ui flag) when using Qt dialogs; instead, the UI is entirely defined as part of the Qt widget - this
    includes behaviour, naming, localization, and mapping the parameter values.

    When specifying the P_AUTO_UI_QT flag on a parameter block descriptor, the usual (Win32) UI specification (dialog resource ID, etc.) are not 
    provided; no UI-definition parameters are to be provided in the paramter block descriptor. When using the P_MULTIMAP flag, the number of parameter
    maps and their individual IDs are provided as usual, but the IDs are not followed by the usual (Win32) UI specifications. Instead, Qt parameter
    maps go through ClassDesc2::CreateQtWidget() to retrieve a new Qt widget and associated properties which, in the Win32 world, would have been
    provided in the parameter block descriptor.

    \par Mapping Qt controls to parameters in the parameter block
    Whereas Win32 controls are mapped to parameters using the p_ui tag, Qt controls are matched by object name (QObject::setObjectName()) with
    the parameter's internal (non-localized) name. For each parameter in the parameter block, a search is performed to find all child Qt objects 
    with a matching name:
    <code> qtDialog.findChildren<QObjet*>(param_def.int_name) </code>
    Every QObject found will be managed by the parameter map if it is deemed compatible (see below). Multiple Qt controls may be mapped to a single
    parameter by giving them the same object name; for example, a parameter could be mapped to both a slider and a spinner control.

    \par Compatible Parameter Types and Qt Control Types
        - <b>All integer and floating-point</b> parameter types are supported via generic handling through QVariant:
            - <b>QComboBox</b>
                - The itemData (QComboBox::setItemData()) is the value to set in the parameter block when a selection is made.
            - <b>QButtonGroup</b>
                - The QButtonGroup groups a set of radio buttons that control an integer parameter value.
                - The ID of the button (QButtonGroup::setId()) is the value used in the parameter block.
            - <b>All Others</b>
                - Any QObject that defines a property "value" is supported.
                - Any QObject that does not define a property "value", but has property "text". The value will be reported as read-only
                  through the "text" property.
        - <b>TYPE_BOOL</b>:
            - Any widget which has a property called "checked". 
            - If the widget has a property called "checkable", then this property's value must be 'true'.
        - <b>TYPE_INODE</b>:
            - <b>QPushButton</b>
                - Will behave as a node picker. The p_classID and p_sclassID options may be used for filtering nodes. The text on the button
                  is automatically set, by the system, to the node's name, if and only if the P_NO_AUTO_LABELS flag is absent from the paremeter
                  definition.
        - <b>All other types</b> are currently unsupported.
*/

//! \brief Creates a parameter map which manages a Qt dialog, in the Render Settings panel or in the Environment panel.
/*!  For more information on using Qt widgets with parameter maps, see: \ref parammap_qt.
    \param map_id The ID of the parameter map, useful if multiple rollups should be created for the same parameter block. Pass 0 if not creating multiple maps.
    \param pb The param block to be controlled by the parameter map.
    \param ip The interface passed to Renderer::CreateParamDialog().
    \param qtDialog The Qt widget which forms the dialog to be controlled. This widget will be inserted into the rollout. The parameter map assumes the ownership of the widget and will delete when it gets destroyed.
    \param title The title of the rollout.
    \param flags Rollout flags, such as APPENDROLL_CLOSED.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The parameter map which controls the relationship between the UI dialog and the parameter block. */
PB2Export IParamMap2* CreateRParamMap2(
    MapID map_id, 
    IParamBlock2 *pb,
    IRendParams *ip,
    MaxSDK::QMaxParamBlockWidget& qtDialog,
    const MCHAR *title,
    DWORD flags,
    int category = ROLLUP_CAT_STANDARD);

//! \brief Creates a parameter map which manages a Qt dialog, in the Render Settings panel or in the Environment panel.
/*!  For more information on using Qt widgets with parameter maps, see: \ref parammap_qt.
    \param map_id The ID of the parameter map, useful if multiple rollups should be created for the same parameter block. Pass 0 if not creating multiple maps.
    \param pb The param block to be controlled by the parameter map.
    \param ip The interface passed to Renderer::CreateParamDialog().
    \param qtDialog The Qt widget which forms the dialog to be controlled. This widget will be inserted into the rollout. The parameter map assumes the ownership of the widget and will delete when it gets destroyed.
    \param tabID Identifier for the tab in which the rollup is to be added. This is the ID used to create new tabs
        in implementations of ITabDialogObject::AddTabToDialog().
    \param title The title of the rollout.
    \param flags Rollout flags, such as APPENDROLL_CLOSED.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The parameter map which controls the relationship between the UI dialog and the parameter block. */
PB2Export IParamMap2* CreateRParamMap2(
	MapID map_id,
	IParamBlock2 *pb,
	IRendParams *ip,
	MaxSDK::QMaxParamBlockWidget& qtDialog,
	const Class_ID& tabID,
	const MCHAR *title,
	DWORD flags,
	int category = ROLLUP_CAT_STANDARD);

/*! \remarks Creates a parameter map to handle the display of render
parameters or atmospheric plug-in parameters.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>IRendParams *ip</b>\n\n
Pass in the plug-ins rendering parameters interface pointer. See
Class IRendParams.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>DLGTEMPLATE *dlgTemplate</b>\n\n
The dialog template.\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the rollup page title bar.\n\n
<b>DWORD flags</b>\n\n
A flag to control the settings of the rollup page:\n\n
<b>APPENDROLL_CLOSED</b>\n\n
Starts the page in the rolled up state.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.\n\n
<b>int category = ROLLUP_CAT_STANDARD</b>\n\n
The category parameter provides flexibility with regard to where a particular
rollup should be displayed in the UI. RollupPanels with lower category fields
will be displayed before RollupPanels with higher category fields. For
RollupPanels with equal category value the one that was added first will be
displayed first. Allthough it is possible to pass any int value as category
there exist currently 5 different category defines: <b>ROLLUP_CAT_SYSTEM</b>,
<b>ROLLUP_CAT_STANDARD</b>, and <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
category sticky, meaning it will not read the category from the
<b>RollupOrder.cfg</b> file, but rather save the category field that was passed
as argument in the <b>CatRegistry</b> and in the <b>RollupOrder.cfg</b>
file.\n\n
The method will take the category of the replaced rollup in case the flags
argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
this system works with param maps as well.
\return  Returns a pointer to the parameter map2. */
inline IParamMap2 *CreateRParamMap2(
		IParamBlock2 *pb,
		IRendParams *ip,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		int category = ROLLUP_CAT_STANDARD) { return CreateRParamMap2(0, pb, ip, hInst, dlgTemplate, title, flags, dlgProc, category); }

/*! \remarks This function destroys a parameter map created by <b>CreateRParamMap2()</b>.
The rollup page is removed and the parameter map is deleted.
\par Parameters:
<b>IParamMap2 *m</b>\n\n
Points to the parameter map2 to destroy. */
PB2Export void DestroyRParamMap2(IParamMap2 *m);

// create a parameter map for render or atmos params in a child dialog window
// of the given parent parammap, used typically to create tab child windows in 
// a tabbed rollout
/*! \remarks Creates a parameter map for render or atmospheric parameters in a child dialog
window of the given parent parammap. This overload of
<b>CreateChildRParamMap2()</b> has a new parameter, <b>map_id</b>, that
specifies the ID of the parameter map/rollup to be created for this particular
parameter block. See original function for the rest of the description. */
PB2Export IParamMap2* CreateChildRParamMap2(
		MapID map_id, 
		IParamBlock2 *pb, 
		IRendParams *ip, 
		HINSTANCE hInst, 
		IParamMap2* parent,
		const MCHAR *dlgTemplate, 
		const MCHAR *title, 
		ParamMap2UserDlgProc* dlgProc=NULL);
/*! \remarks This function create a parameter map for render or atmospheric
parameters in a child dialog window of the given parent parammap, used
typically to create tab child windows in a tabbed rollout. This version takes
an extra parent IParamMap2* and creates a child dialog window in the parent
parammap's window (rather than a new rollup) that is mapped by the new
parammap. Developers need to call this explicitly once the parent parammap has
been created as child parammaps are not created automatically by the
<b>P_AUTO_UI</b> mechanisms.\n\n
Note, in version 4.0 and later, this actually maps to a call on the explicit
map ID overload of <b>CreateChildRParamMap2()</b> with default map ID of 0.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>IRendParams *ip</b>\n\n
Pass in the plug-ins material parameters interface pointer. See
Class IRendParams.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>IParamMap2* parent</b>\n\n
The parent parameter map.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
Dialog template for the rollup page (created using the resource editor)\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the dialog.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.
\return  Returns a pointer to the parameter map2 created. */
inline IParamMap2* CreateChildRParamMap2(
		IParamBlock2 *pb, 
		IRendParams *ip, 
		HINSTANCE hInst, 
		IParamMap2* parent,
		const MCHAR *dlgTemplate, 
		const MCHAR *title, 
		ParamMap2UserDlgProc* dlgProc=NULL) { return CreateChildRParamMap2(0, pb, ip, hInst, parent, dlgTemplate, title, dlgProc); }

PB2Export IParamMap2* CreateChildRParamMap2(
		MapID map_id, 
		IParamBlock2 *pb, 
		IRendParams *ip, 
		HINSTANCE hInst, 
		IParamMap2* parent,
		DLGTEMPLATE *dlgTemplate, 
		const MCHAR *title, 
		ParamMap2UserDlgProc* dlgProc=NULL);
/*! \remarks This function create a parameter map for render or atmospheric parameters in a
child dialog window of the given parent parammap, used typically to create tab
child windows in a tabbed rollout. This version takes an extra parent
IParamMap2* and creates a child dialog window in the parent parammap's window
(rather than a new rollup) that is mapped by the new parammap. Developers need
to call this explicitly once the parent parammap has been created as child
parammaps are not created automatically by the <b>P_AUTO_UI</b> mechanisms.
This function is currently not being used.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>IRendParams *ip</b>\n\n
Pass in the plug-ins material parameters interface pointer. See
Class IRendParams.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>IParamMap2* parent</b>\n\n
The parent parameter map.\n\n
<b>DLGTEMPLATE *dlgTemplate</b>\n\n
Dialog template for the rollup page (created using the resource editor)\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the dialog.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.
\return  Returns a pointer to the parameter map2 created. */
inline IParamMap2* CreateChildRParamMap2(
		IParamBlock2 *pb, 
		IRendParams *ip, 
		HINSTANCE hInst, 
		IParamMap2* parent,
		DLGTEMPLATE *dlgTemplate, 
		const MCHAR *title, 
		ParamMap2UserDlgProc* dlgProc=NULL) { return CreateChildRParamMap2(0, pb, ip, hInst, parent, dlgTemplate, title, dlgProc); }

/*! \remarks This function destroys a parameter map created by
<b>CreateChildRParamMap2()</b>.
\par Parameters:
<b>IParamMap2 *m</b>\n\n
Points to the parameter map2 to destroy. */
PB2Export void DestroyChildRParamMap2(IParamMap2 *m);

// Creates a parameter map to handle the display of texture map or
// material parameters in the material editor.
/*! \remarks Creates a parameter map to handle the display of texture map or material
parameters in the material editor. This overload of <b>CreateMParamMap2()</b>
has a new parameter, <b>map_id</b>, that specifies the ID of the parameter
map/rollup to be created for this particular parameter block. See original
function for the rest of the description. */
PB2Export IParamMap2 *CreateMParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		IMtlParams *ip,
		HINSTANCE hInst,
		HWND hmedit,
		TexDADMgr* tdad,
		MtlDADMgr* mdad,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		HWND hOldRollup=NULL,
		int category = ROLLUP_CAT_STANDARD);
/*! \remarks Creates a parameter map to handle the display of texture map or
material parameters in the material editor.\n\n
Note, in version 4.0 and later, this actually maps to a call on the explicit
map ID overload of <b>CreateMParamMap2()</b> with default map ID of 0.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>IMtlParams *ip</b>\n\n
Pass in the plug-ins material parameters interface pointer. See
Class IMtlParams.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>HWND hmedit</b>\n\n
The window handle to the materials editor.\n\n
<b>TexDADMgr* dad</b>\n\n
Points to the manager used to handle drag and drop of textures. See
Class TexDADMgr.\n\n
<b>MtlDADMgr* mdad</b>\n\n
Points to the manager used to handle drag and drop of materials. See
Class MtlDADMgr.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
Dialog template for the rollup page (created using the resource editor)\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the rollup page title bar.\n\n
<b>DWORD flags</b>\n\n
A flag to control the settings of the rollup page:\n\n
<b>APPENDROLL_CLOSED</b>\n\n
Starts the page in the rolled up state.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.\n\n
<b>HWND hOldRollup=NULL</b>\n\n
If non-NULL specifies an existing rollup window in the current UI context that
should be replaced with the newly created rollup for this map.\n\n
<b>int category = ROLLUP_CAT_STANDARD</b>\n\n
The category parameter provides flexibility with regard to where a particular
rollup should be displayed in the UI. RollupPanels with lower category fields
will be displayed before RollupPanels with higher category fields. For
RollupPanels with equal category value the one that was added first will be
displayed first. Allthough it is possible to pass any int value as category
there exist currently 5 different category defines: <b>ROLLUP_CAT_SYSTEM</b>,
<b>ROLLUP_CAT_STANDARD</b>, and <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
category sticky, meaning it will not read the category from the
<b>RollupOrder.cfg</b> file, but rather save the category field that was passed
as argument in the <b>CatRegistry</b> and in the <b>RollupOrder.cfg</b>
file.\n\n
The method will take the category of the replaced rollup in case the flags
argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
this system works with param maps as well.
\return  Returns a pointer to the parameter map2 created. */
inline IParamMap2 *CreateMParamMap2(
		IParamBlock2 *pb,
		IMtlParams *ip,
		HINSTANCE hInst,
		HWND hmedit,
		TexDADMgr* tdad,
		MtlDADMgr* mdad,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		HWND hOldRollup=NULL,
		int category = ROLLUP_CAT_STANDARD) { return CreateMParamMap2(0, pb, ip, hInst, hmedit, tdad, mdad, dlgTemplate, title, flags, dlgProc, hOldRollup, category); }

PB2Export IParamMap2 *CreateMParamMap2(
		MapID map_id, 
		IParamBlock2 *pb,
		IMtlParams *ip,
		HINSTANCE hInst,
		HWND hmedit,
		TexDADMgr* tdad,
		MtlDADMgr* mdad,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		HWND hOldRollup=NULL,
		int category = ROLLUP_CAT_STANDARD);

//! \brief Creates a parameter map which manages a Qt dialog, in the material editor.
/*! For more information on using Qt widgets with parameter maps, see: \ref parammap_qt.
    \param map_id The ID of the parameter map, useful if multiple rollups should be created for the same parameter block. Pass 0 if not creating multiple maps.
    \param pb The param block to be controlled by the parameter map.
    \param ip The interface passed to MtlBase::CreateParamDlg()
    \param hmedit The handle passed to MtlBase::CreateParamDlg()
    \param tdad An optional interface to manage drag-and-dropping of Texmap parameters.
    \param mdad An optional interface to manage drag-and-dropping of Mtl parameters.
    \param qtDialog The Qt widget which forms the dialog to be controlled. This widget will be inserted into the rollout. The parameter map assumes the ownership of the widget and will delete when it gets destroyed.
    \param title The title of the rollout.
    \param flags Rollout flags, such as APPENDROLL_CLOSED.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The parameter map which controls the relationship between the UI dialog and the parameter block. */
PB2Export IParamMap2* CreateMParamMap2(
    MapID map_id, 
    IParamBlock2 *pb,
    IMtlParams *ip,
    HWND hmedit,
    TexDADMgr* tdad,
    MtlDADMgr* mdad,
    MaxSDK::QMaxParamBlockWidget& qtDialog,
    const MCHAR *title,
    DWORD flags,
    int category = ROLLUP_CAT_STANDARD);

/*! \remarks Creates a parameter map to handle the display of texture map or material
parameters in the material editor. This function is currently not in use.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>IMtlParams *ip</b>\n\n
Pass in the plug-ins material parameters interface pointer. See
Class IMtlParams.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>HWND hmedit</b>\n\n
The window handle to the materials editor.\n\n
<b>TexDADMgr* dad</b>\n\n
Points to the manager used to handle drag and drop of textures. See
Class TexDADMgr.\n\n
<b>MtlDADMgr* mdad</b>\n\n
Points to the manager used to handle drag and drop of materials. See
Class MtlDADMgr.\n\n
<b>DLGTEMPLATE *dlgTemplate</b>\n\n
Dialog template for the rollup page (created using the resource editor)\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the rollup page title bar.\n\n
<b>DWORD flags</b>\n\n
A flag to control the settings of the rollup page:\n\n
<b>APPENDROLL_CLOSED</b>\n\n
Starts the page in the rolled up state.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.\n\n
<b>HWND hOldRollup=NULL</b>\n\n
If non-NULL specifies an existing rollup window in the current UI context that
should be replaced with the newly created rollup for this map.\n\n
<b>int category = ROLLUP_CAT_STANDARD</b>\n\n
The category parameter provides flexibility with regard to where a particular
rollup should be displayed in the UI. RollupPanels with lower category fields
will be displayed before RollupPanels with higher category fields. For
RollupPanels with equal category value the one that was added first will be
displayed first. Allthough it is possible to pass any int value as category
there exist currently 5 different category defines: <b>ROLLUP_CAT_SYSTEM</b>,
<b>ROLLUP_CAT_STANDARD</b>, and <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
category sticky, meaning it will not read the category from the
<b>RollupOrder.cfg</b> file, but rather save the category field that was passed
as argument in the <b>CatRegistry</b> and in the <b>RollupOrder.cfg</b>
file.\n\n
The method will take the category of the replaced rollup in case the flags
argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
this system works with param maps as well.
\return  Returns a pointer to the parameter map2 created. */
inline IParamMap2 *CreateMParamMap2(
		IParamBlock2 *pb,
		IMtlParams *ip,
		HINSTANCE hInst,
		HWND hmedit,
		TexDADMgr* tdad,
		MtlDADMgr* mdad,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		HWND hOldRollup=NULL,
		int category = ROLLUP_CAT_STANDARD) { return CreateMParamMap2(0, pb, ip, hInst, hmedit, tdad, mdad, dlgTemplate, title, flags, dlgProc, hOldRollup, category); }

/*! \remarks This function destroys a parameter map created by
<b>CreateMParamMap2()</b>. The rollup page is removed and the parameter map is
deleted.
\par Parameters:
<b>IParamMap2 *m</b>\n\n
Points to the parameter map2 to destroy. */
PB2Export void DestroyMParamMap2(IParamMap2 *m);

// create a child dialog of the given parent parammap (for tabbed dialogs, etc.)
/*! \remarks This function creates a child dialog of the given modal parent parammap (for
tabbed dialogs, etc.). Unlike modal parent parammap, you do need to destroy it.
This version takes an extra parent IParamMap2* and creates a child dialog
window in the parent parammap's window (rather than a new rollup) that is
mapped by the new parammap. Developers need to call this explicitly once the
parent parammap has been created as child parammaps are not created
automatically by the <b>P_AUTO_UI</b> mechanisms.
\par Parameters:
<b>MapID map_id</b>\n\n
Specifies the ID of the parameter map/rollup to be created for this particular
parameter block.\n\n
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>IParamMap2* parent</b>\n\n
The parent parameter map.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
The dialog template.\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the dialog.\n\n
<b>ParamMap2UserDlgProc *proc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.
\return  Returns TRUE if the user selected OK, FALSE otherwise. */
PB2Export IParamMap2 *CreateChildMParamMap2(
		MapID map_id, 
		IParamBlock2 *pb, 
		IMtlParams *ip, 
		HINSTANCE hInst, 
		IParamMap2* parent,
		HWND hmedit,
		TexDADMgr* tdad,
		MtlDADMgr* mdad,
		const MCHAR *dlgTemplate, 
		const MCHAR *title, 
		ParamMap2UserDlgProc* dlgProc=NULL);
/*! \remarks This function creates and returns a child dialog of the given
parent parammap (for tabbed dialogs, etc.). This version takes an extra parent
IParamMap2* and create a child dialog window in the parent parammap's window
(rather than a new rollup) that is mapped by the new parammap. Developers need
to call this explicitly once the parent parammap has been created as child
parammaps are not created automatically by the <b>P_AUTO_UI</b> mechanisms.\n\n
Note, in version 4.0 and later, this actually maps to a call on the explicit
map ID overload of <b>CreateChildMParamMap2()</b> with default map ID of 0.
\par Parameters:
<b>IParamBlock2 *pb</b>\n\n
Points to the parameter block2.\n\n
<b>IMtlParams *ip</b>\n\n
Pass in the plug-ins material parameters interface pointer. See
Class IMtlParams.\n\n
<b>HINSTANCE hInst</b>\n\n
The plug-ins instance handle.\n\n
<b>IParamMap2* parent</b>\n\n
The parent parameter map.\n\n
<b>TexDADMgr* tdad</b>\n\n
Points to the manager used to handle drag and drop of textures. See
Class TexDADMgr.\n\n
<b>MtlDADMgr* mdad</b>\n\n
Points to the manager used to handle drag and drop of materials. See
Class MtlDADMgr_.\n\n
<b>MCHAR *dlgTemplate</b>\n\n
Dialog template for the rollup page (created using the resource editor)\n\n
<b>MCHAR *title</b>\n\n
The title displayed in the dialog.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.
\return  Returns a pointer to the parameter map2 created. */
inline IParamMap2 *CreateChildMParamMap2(
		IParamBlock2 *pb,
		IMtlParams *ip,
		HINSTANCE hInst,
		IParamMap2* parent,
		HWND hmedit,
		TexDADMgr* tdad,
		MtlDADMgr* mdad,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc* dlgProc=NULL) { return CreateChildMParamMap2(0, pb, ip, hInst, parent, hmedit, tdad, mdad, dlgTemplate, title, dlgProc); }

PB2Export IParamMap2 *CreateChildMParamMap2(
		MapID map_id, 
		IParamBlock2 *pb, 
		IMtlParams *ip, 
		HINSTANCE hInst, 
		IParamMap2* parent,
		HWND hmedit,
		TexDADMgr* tdad,
		MtlDADMgr* mdad,
		DLGTEMPLATE *dlgTemplate, 
		const MCHAR *title, 
		ParamMap2UserDlgProc* dlgProc=NULL);
inline IParamMap2 *CreateChildMParamMap2(
		IParamBlock2 *pb,
		IMtlParams *ip,
		HINSTANCE hInst,
		IParamMap2* parent,
		HWND hmedit,
		TexDADMgr* tdad,
		MtlDADMgr* mdad,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		ParamMap2UserDlgProc* dlgProc=NULL) { return CreateChildMParamMap2(0, pb, ip, hInst, parent, hmedit, tdad, mdad, dlgTemplate, title, dlgProc); }

/*! \remarks This function destroys a parameter map created by
<b>CreateChildMParamMap2()</b>.
\par Parameters:
<b>IParamMap2 *m</b>\n\n
Points to the parameter map2 to destroy. */
PB2Export void DestroyChildMParamMap2(IParamMap2 *m);

//--------------------------------------- Bayboro 02-22-02
// Creates a parameter map to handle the display of parameters in a floating rollup window.
// This will add a rollup page to the modal/modeless rollup window.
// DestroyRWParamMap().
PB2Export IParamMap2 *CreateRWParamMap2(
		MapID map_id,
		IParamBlock2 *pb,
		IRollupWindow* rw,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		BOOL bindPBlockToMap=TRUE);
inline IParamMap2 *CreateRWParamMap2(
		IParamBlock2 *pb,
		IRollupWindow* rw,
		HINSTANCE hInst,
		const MCHAR *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		BOOL bindPBlockToMap=TRUE) { return CreateRWParamMap2(0, pb, rw, hInst, dlgTemplate, title, flags, dlgProc, bindPBlockToMap); }

PB2Export IParamMap2 *CreateRWParamMap2(
		MapID map_id,
		IParamBlock2 *pb,
		IRollupWindow* rw,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		BOOL bindPBlockToMap = TRUE);
inline IParamMap2 *CreateRWParamMap2(
		IParamBlock2 *pb,
		IRollupWindow* rw,
		HINSTANCE hInst,
		DLGTEMPLATE *dlgTemplate,
		const MCHAR *title,
		DWORD flags,
		ParamMap2UserDlgProc* dlgProc=NULL,
		BOOL bindPBlockToMap=TRUE) { return CreateRWParamMap2(0, pb, rw, hInst, dlgTemplate, title, flags, dlgProc, bindPBlockToMap); }

PB2Export void DestroyRWParamMap2(IParamMap2 *m);
//--------------------------------end of Bayboro 02-22-02

//==================================================================================================
// class IAutoMParamDlg
//
//  Auto ParamDlg class for Material Editor auto-UI, instanced by ClassDesc2::CreateParamDlg()
//  It maintains a table of secondary ParamDlgs for master ParamDlgs (eg, the one returned 
//  from CreateParamDlg()) and will broadcast appropriate method calls to them
//  as the master receives them
/*! \sa  Class ParamDlg, Class IParamMap2, Class IMtlParams, Class ParamMap2UserDlgProc.\n\n
\par Description:
This is the Auto ParamDlg class for Material Editor auto-UI, instanced by
<b>ClassDesc2::CreateParamDlg()</b>. It maintains a table of secondary
ParamDlgs for master ParamDlgs (eg, the one returned from CreateParamDlg()) and
will broadcast appropriate method calls to them as the master receives them.
 */
class IAutoMParamDlg : public ParamDlg
{
	public:
		/*! \remarks This method causes the user interface controls to be
		re-drawn. */
		virtual void		InvalidateUI()=0;
		/*! \remarks This method may be called to causes the viewports to be
		redrawn. It should be called when any parameter that affects the look
		of the material in the viewport has been altered. If the material is
		not on a visible node in a shaded view, nothing will happen. This
		method should <b>not</b> be called as a spinner is being dragged, but
		only upon release of the mouse button. */
		virtual void		MtlChanged()=0;
		// secondary dialog list management
		/*! \remarks Returns the number of secondary dialogs. */
		virtual int			NumDlgs()=0;
		/*! \remarks Adds the specified dialog as another secondary dialog.
		\par Parameters:
		<b>ParamDlg* dlg</b>\n\n
		Points to the parameter dialog to add. */
		virtual void		AddDlg(ParamDlg* dlg)=0;
		/*! \remarks Returns a pointer to the 'i-th' secondary dialog.
		\par Parameters:
		<b>int i</b>\n\n
		The zero based index of the dialog to return. */
		virtual ParamDlg*	GetDlg(int i)=0;
		/*! \remarks Sets the 'i-th' dialog to the one passed.
		\par Parameters:
		<b>int i</b>\n\n
		The zero based index of the dialog to set.\n\n
		<b>ParamDlg* dlg</b>\n\n
		Points to the parameter dialog to set. */
		virtual void		SetDlg(int i, ParamDlg* dlg)=0;
		/*! \remarks This method is used for deleting secondary dialogs from a
		master <b>IAutoMParamDlg</b>. Use this along with <b>AddDlg()</b> if
		you are dynamically changing the set of rollups for the plugin, so that
		the <b>P_AUTO_UI</b> system can correctly manage all current secondary
		rollups.
		\par Parameters:
		<b>ParamDlg* dlg</b>\n\n
		Points to the ParamDlg to delete. */
		virtual void		DeleteDlg(ParamDlg* dlg)=0;
		// access to this dlg's parammap stuff
		/*! \remarks Returns a pointer to the parameter map2 of this primary
		(master) dialog. */
		virtual IParamMap2* GetMap()=0;
};
// create an AutoMParamDlg for material editor
/*! \remarks This function may be called to create a parameter map2 for use in the material
editor dialog. This overload of <b>CreateAutoMParamDlg()</b> has a new
parameter, <b>map_id</b>, that specifies the ID of the parameter map/rollup to
be created for this particular parameter block. See original function for the
rest of the description. */
PB2Export IAutoMParamDlg* CreateAutoMParamDlg(MapID map_id, HWND hMedit, IMtlParams *i, ReferenceTarget* obj,
											  IParamBlock2* pb, ClassDesc2* cd, HINSTANCE inst, 
											  const MCHAR* dlgTemplate, const MCHAR* title, int rollFlags,
											  ParamMap2UserDlgProc* dlgProc=NULL,
											  HWND hOldRollup=NULL,
											  int category = ROLLUP_CAT_STANDARD);
/*! \remarks This function may be called to create a parameter map2 for use in
the material editor dialog.\n\n
Note, in version 4.0 and later, this actually maps to a call on the explicit
map ID overload of <b>CreateAutoMParamDlg()</b> with default map ID of 0.
\par Parameters:
<b>HWND hMedit</b>\n\n
The window handle of the materials editor.\n\n
<b>IMtlParams *i</b>\n\n
The interface pointer for materials. See
Class IMtlParams.\n\n
<b>MtlBase* mtl</b>\n\n
Points to the material calling this function.\n\n
<b>IParamBlock2* pb</b>\n\n
Points to the parameter block instance associated with the parameter map.\n\n
<b>ClassDesc2* cd</b>\n\n
The class descriptor2 for the plug-in creating the parameter map.\n\n
<b>HINSTANCE inst</b>\n\n
The plug-ins DLL instance handle.\n\n
<b>MCHAR* dlgTemplate</b>\n\n
Dialog template for the rollup page (created using the resource editor)\n\n
<b>MCHAR* title</b>\n\n
The title displayed in the rollup page title bar.\n\n
<b>int rollFlags</b>\n\n
A set of flags to control settings of the rollup page.\n\n
<b>APPENDROLL_CLOSED</b>\n\n
Starts the page in the rolled up state.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.\n\n
<b>HWND hOldRollup=NULL</b>\n\n
An optional window handle to allow supplying an existing rollup that will be
<b>replaced</b> by the newly cerated one.
\return  A pointer to an interface for managing the parameter map2. */
inline IAutoMParamDlg* CreateAutoMParamDlg(HWND hMedit, IMtlParams *i, ReferenceTarget* obj,
											  IParamBlock2* pb, ClassDesc2* cd, HINSTANCE inst, 
											  const MCHAR* dlgTemplate, const MCHAR* title, int rollFlags,
											  ParamMap2UserDlgProc* dlgProc=NULL,
											  HWND hOldRollup=NULL,
											  int category = ROLLUP_CAT_STANDARD)
											{
												return CreateAutoMParamDlg(0, hMedit, i, obj, pb, cd, inst, dlgTemplate, 
																		   title, rollFlags, dlgProc, hOldRollup, category); 
											}

PB2Export IAutoMParamDlg* CreateAutoMParamDlg(MapID map_id, HWND hMedit, IMtlParams *i, ReferenceTarget* obj,
											  IParamBlock2* pb, ClassDesc2* cd, HINSTANCE inst, 
											  DLGTEMPLATE* dlgTemplate, const MCHAR* title, int rollFlags,
											  ParamMap2UserDlgProc* dlgProc=NULL,
											  HWND hOldRollup=NULL,
											  int category = ROLLUP_CAT_STANDARD);
inline IAutoMParamDlg* CreateAutoMParamDlg(HWND hMedit, IMtlParams *i, ReferenceTarget* obj,
											  IParamBlock2* pb, ClassDesc2* cd, HINSTANCE inst, 
											  DLGTEMPLATE* dlgTemplate, const MCHAR* title, int rollFlags,
											  ParamMap2UserDlgProc* dlgProc=NULL,
											  HWND hOldRollup=NULL,
											  int category = ROLLUP_CAT_STANDARD)
											{
												return CreateAutoMParamDlg(0, hMedit, i, obj, pb, cd, inst, dlgTemplate, 
																		   title, rollFlags, dlgProc, hOldRollup, category); 
											}

//! \brief Creates a Qt-based ParamDlg, for use in the material editor, which automatically manages the creation and lifetime of the underlying parameter map.
/*!
    In most cases, this method shouldn't be used directly by plugin developers. Instead, the plugin should call ClassDesc2::CreateParamDlgs()
    to automatically create the ParamDlg for all dialogs flagged with P_AUTO_UI or P_AUTO_UI_QT. But if the plugin needs to manage (perhaps conditionally)
    the creation of its dialogs, then it may call this function directly for individual parameter maps.

    \param map_id The ID of the parameter map for which to create the dialog.
    \param hMedit The handle passed to MtlBase::CreateParamDlg()
    \param i The interface passed to MtlBase::CreateParamDlg()
    \param obj The object (the MtlBase) for which the dialog is being created.
    \param pb The parameter block to which the parameter map is associated.
    \param cd The class descriptor of the object for which the dialog is being created.
    \param qtWidget The Qt widget that will be shown in the dialog.
    \param title The title of the rollup window to be created.
    \param rollFlags Rollout flags, such as APPENDROLL_CLOSED.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The ParamDlg which was created. 
    \sa \ref parammap_qt */
PB2Export IAutoMParamDlg* CreateAutoMParamDlg(MapID map_id, HWND hMedit, IMtlParams *i, ReferenceTarget* obj,
                                              IParamBlock2* pb, ClassDesc2* cd,
                                              MaxSDK::QMaxParamBlockWidget& qtWidget, const MCHAR* title, int rollFlags,
                                              int category = ROLLUP_CAT_STANDARD);


//==================================================================================================
// class IAutoEParamDlg
//
//  Auto ParamDlg class for SpecialFX auto-UI, instanced by ClassDesc2::CreateParamDialog()
//  It maintains a table of secondary EffectParamDlg for master EffectParamDlg (eg, the one returned 
//  from CreateParamDialog()) and will broadcast appropriate method calls to them
//  as the master receives them
/*! \sa  Class ParamDlg, Class IParamMap2, Class IRendParams, Class ParamMap2UserDlgProc.\n\n
\par Description:
Auto ParamDlg class for Effects auto-UI, instanced by
<b>ClassDesc2::CreateParamDialog()</b>. It maintains a table of secondary
EffectParamDlg for master EffectParamDlg (e.g., the one returned from
CreateParamDialog()) and will broadcast appropriate method calls to them as the
master receives them.  */
class IAutoEParamDlg : public SFXParamDlg
{
	public:
		/*! \remarks This method causes the user interface controls to be
		re-drawn. */
		virtual void		InvalidateUI()=0;
		// secondary dialog list management
		/*! \remarks Returns the number of secondary dialogs. */
		virtual int			NumDlgs()=0;
		/*! \remarks Adds the specified dialog as another secondary dialog.
		\par Parameters:
		<b>SFXParamDlg* dlg</b>\n\n
		Points to the parameter dialog to add. */
		virtual void		AddDlg(SFXParamDlg* dlg)=0;
		/*! \remarks Returns a pointer to the 'i-th' secondary dialog.
		\par Parameters:
		<b>int i</b>\n\n
		The zero based index of the dialog to return. */
		virtual SFXParamDlg* GetDlg(int i)=0;
		/*! \remarks Sets the 'i-th' dialog to the one passed.
		\par Parameters:
		<b>int i</b>\n\n
		The zero based index of the dialog to set.\n\n
		<b>SFXParamDlg* dlg</b>\n\n
		Points to the parameter dialog to set. */
		virtual void		SetDlg(int i, SFXParamDlg* dlg)=0;
		/*! \remarks This method is used for deleting secondary dialogs from a
		master <b>IAutoEParamDlg</b>. Use this along with <b>AddDlg()</b> if
		you are dynamically changing the set of rollups for the plugin, so that
		the <b>P_AUTO_UI</b> system can correctly manage all current secondary
		rollups.
		\par Parameters:
		<b>SFXParamDlg* dlg</b>\n\n
		Points to the ParamDlg to delete. */
		virtual void		DeleteDlg(SFXParamDlg* dlg)=0;
		// access to this dlg's parammap stuff
		/*! \remarks Returns a pointer to the parameter map2 of this primary
		(master) dialog. */
		virtual IParamMap2* GetMap()=0;
};

/*! \remarks This function creates an AutoEParamDlg for render effects. This overload of
<b>CreateAutoEParamDlg()</b> has a new parameter, <b>map_id</b>, that specifies
the ID of the parameter map/rollup to be created for this particular parameter
block. See original function for the rest of the description. */
PB2Export IAutoEParamDlg* CreateAutoEParamDlg(MapID map_id, IRendParams *i, SpecialFX* e,
											  IParamBlock2* pb, ClassDesc2* cd, HINSTANCE inst, 
											  const MCHAR* dlgTemplate, const MCHAR* title, int rollFlags, 
											  ParamMap2UserDlgProc* dlgProc=NULL,
											  int category = ROLLUP_CAT_STANDARD );
/*! \remarks This function creates an AutoEParamDlg for render effects.\n\n
Note, in version 4.0 and later, this actually maps to a call on the explicit
map ID overload of <b>CreateAutoEParamDlg()</b> with default map ID of 0.
\par Parameters:
<b>IRendParams *i</b>\n\n
An interface pointer for rendering effects.\n\n
<b>SpecialFX* e</b>\n\n
Points to the rendering effect calling this function.\n\n
<b>IParamBlock2* pb</b>\n\n
Points to the parameter block instance associated with the parameter map.\n\n
<b>ClassDesc2* cd</b>\n\n
The class descriptor2 for the plug-in creating the parameter map.\n\n
<b>HINSTANCE inst</b>\n\n
The plug-ins DLL instance handle.\n\n
<b>MCHAR* dlgTemplate</b>\n\n
Dialog template for the rollup page (created using the resource editor)\n\n
<b>MCHAR* title</b>\n\n
The title displayed in the rollup page title bar.\n\n
<b>int rollFlags</b>\n\n
A set of flags to control settings of the rollup page.\n\n
<b>APPENDROLL_CLOSED</b>\n\n
Starts the page in the rolled up state.\n\n
<b>ParamMap2UserDlgProc* dlgProc=NULL</b>\n\n
If there is some custom handling required by a particular control, the client
can derive a class from ParamMap2UserDlgProc and set it as the parameter map's
user callback.
\return  A pointer to an interface for managing the parameter map2. */
inline IAutoEParamDlg* CreateAutoEParamDlg(IRendParams *i, SpecialFX* e,
											  IParamBlock2* pb, ClassDesc2* cd, HINSTANCE inst, 
											  const MCHAR* dlgTemplate, const MCHAR* title, int rollFlags, 
											  ParamMap2UserDlgProc* dlgProc=NULL,
											  int category = ROLLUP_CAT_STANDARD ) 
											{ 
												return CreateAutoEParamDlg(0, i, e, pb, cd, inst, dlgTemplate, title, rollFlags, dlgProc, category);
											}


PB2Export IAutoEParamDlg* CreateAutoEParamDlg(MapID map_id, IRendParams *i, SpecialFX* e,
											  IParamBlock2* pb, ClassDesc2* cd, HINSTANCE inst, 
											  DLGTEMPLATE* dlgTemplate, const MCHAR* title, int rollFlags, 
											  ParamMap2UserDlgProc* dlgProc=NULL,
											  int category = ROLLUP_CAT_STANDARD );
inline IAutoEParamDlg* CreateAutoEParamDlg(IRendParams *i, SpecialFX* e,
											  IParamBlock2* pb, ClassDesc2* cd, HINSTANCE inst, 
											  DLGTEMPLATE* dlgTemplate, const MCHAR* title, int rollFlags, 
											  ParamMap2UserDlgProc* dlgProc=NULL,
											  int category = ROLLUP_CAT_STANDARD ) 
											{ 
												return CreateAutoEParamDlg(0, i, e, pb, cd, inst, dlgTemplate, title, rollFlags, dlgProc, category);
											}

//! \brief Creates a Qt-based ParamDlg, for use in the environment panel, which automatically manages the creation and lifetime of the underlying parameter map.
/*!
    In most cases, this method shouldn't be used directly by plugin developers. Instead, the plugin should call ClassDesc2::CreateParamDlgs()
    to automatically create the ParamDlg for all dialogs flagged with P_AUTO_UI or P_AUTO_UI_QT. But if the plugin needs to manage (perhaps conditionally)
    the creation of its dialogs, then it may call this function directly for individual parameter maps.

    \param map_id The ID of the parameter map for which to create the dialog.
    \param i The interface passed to SpecialFX::CreateParamDialog().
    \param e The object for which the dialog is being created.
    \param pb The parameter block to which the parameter map is associated.
    \param cd The class descriptor of the object for which the dialog is being created.
    \param qtWidget The Qt widget that will be shown in the dialog.
    \param title The title of the rollup window to be created.
    \param rollFlags Rollout flags, such as APPENDROLL_CLOSED.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The ParamDlg which was created. 
    \sa \ref parammap_qt */
PB2Export IAutoEParamDlg* CreateAutoEParamDlg(MapID map_id, IRendParams *i, SpecialFX* e,
                                              IParamBlock2* pb, ClassDesc2* cd, 
                                              MaxSDK::QMaxParamBlockWidget& qtWidget, const MCHAR* title, int rollFlags, 
                                              int category = ROLLUP_CAT_STANDARD );


//==================================================================================================
// class IAutoRendererParamDlg
//
//! Implementation of a ParamDlg that handles the automatic creation and lifetime management for 
//! a parameter map of a renderer plugin.
class IAutoRendererParamDlg : public RendParamDlg
{
	public:
        //! Invalidates the UI for every parameter map controlled by this dialog and its sub-dialogs.
		virtual void InvalidateUI() = 0;
        //! Returns the number of sub dialogs being controlled.
		virtual int NumDlgs() const = 0;
        //! Adds a sub-dialog to be controlled by this master dialog.
		virtual void AddDlg(RendParamDlg& dlg) = 0;
        //! Returns the sub-dialog with the given index.
		virtual RendParamDlg* GetDlg(const int i) = 0;
        //! Replaces the dialog, at the given index, with another.
		virtual void SetDlg(const int i, RendParamDlg* dlg) = 0;
        //! Removes the given dialog from the set of sub-dialogs.
		virtual void DeleteDlg(RendParamDlg& dlg)=0;
        //! Returns the parameter map to which this dialog is connected.
		virtual IParamMap2* GetMap() = 0;
};

/*! Creates a Win32-based ParamDlg, for use in the render settings panel, which automatically manages the creation and lifetime of the underlying parameter map.
    
    In most cases, this method shouldn't be used directly by plugin developers. Instead, the plugin should call ClassDesc2::CreateParamDlgs()
    to automatically create the ParamDlg for all dialogs flagged with P_AUTO_UI or P_AUTO_UI_QT. But if the plugin needs to manage (perhaps conditionally)
    the creation of its dialogs, then it may call this function directly for individual parameter maps.

    \param map_id The ID of the parameter map for which to create the dialog.
    \param ip The interface passed to Renderer::CreateParamDialog().
    \param renderer The object for which the dialog is being created.
    \param pb The parameter block to which the parameter map is associated.
    \param cd The class descriptor of the object for which the dialog is being created.
    \param inst The HINSTANCE for the DLL that hosts the resources for the dialog template.
    \param dlgTemplate The dialog template to be used, specified as MAKEINTRESOURCE(resource_id).
    \param title The title of the rollup window to be created.
    \param rollFlags Rollout flags, such as APPENDROLL_CLOSED.
    \param dlgProc An optional dialog procedure to do custom processing of window messages.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The ParamDlg which was created. */
PB2Export IAutoRendererParamDlg* CreateAutoRenderParamDlg(
    MapID map_id, 
    IRendParams *ip,
    Renderer& renderer,
    IParamBlock2* pb, 
    ClassDesc2* cd, 
    HINSTANCE inst, 
    const MCHAR* dlgTemplate, 
    const MCHAR* title, 
    int rollFlags,
    ParamMap2UserDlgProc* dlgProc=NULL,
    int category = ROLLUP_CAT_STANDARD);

/*! Creates a Win32-based ParamDlg, for use in the render settings panel, which automatically manages the creation and lifetime of the underlying parameter map.
    
    In most cases, this method shouldn't be used directly by plugin developers. Instead, the plugin should call ClassDesc2::CreateParamDlgs()
    to automatically create the ParamDlg for all dialogs flagged with P_AUTO_UI or P_AUTO_UI_QT. But if the plugin needs to manage (perhaps conditionally)
    the creation of its dialogs, then it may call this function directly for individual parameter maps.

    \param map_id The ID of the parameter map for which to create the dialog.
    \param ip The interface passed to Renderer::CreateParamDialog().
    \param renderer The object for which the dialog is being created.
    \param pb The parameter block to which the parameter map is associated.
    \param cd The class descriptor of the object for which the dialog is being created.
    \param inst The HINSTANCE for the DLL that hosts the resources for the dialog template.
    \param dlgTemplate The dialog template to be used.
    \param title The title of the rollup window to be created.
    \param rollFlags Rollout flags, such as APPENDROLL_CLOSED.
    \param dlgProc An optional dialog procedure to do custom processing of window messages.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The ParamDlg which was created. */
PB2Export IAutoRendererParamDlg* CreateAutoRenderParamDlg(
    MapID map_id, 
    IRendParams *ip,
    Renderer& renderer,
    IParamBlock2* pb, 
    ClassDesc2* cd, 
    HINSTANCE inst, 
    DLGTEMPLATE* dlgTemplate, 
    const MCHAR* title, 
    int rollFlags,
    ParamMap2UserDlgProc* dlgProc=NULL,
    int category = ROLLUP_CAT_STANDARD);

//! Creates a Qt-based ParamDlg, for use in the render settings panel, which automatically manages the creation and lifetime of the underlying parameter map.
/*! 
    In most cases, this method shouldn't be used directly by plugin developers. Instead, the plugin should call ClassDesc2::CreateParamDlgs()
    to automatically create the ParamDlg for all dialogs flagged with P_AUTO_UI or P_AUTO_UI_QT. But if the plugin needs to manage (perhaps conditionally)
    the creation of its dialogs, then it may call this function directly for individual parameter maps.

    \param map_id The ID of the parameter map for which to create the dialog.
    \param ip The interface passed to Renderer::CreateParamDialog().
    \param renderer The object for which the dialog is being created.
    \param pb The parameter block to which the parameter map is associated.
    \param cd The class descriptor of the object for which the dialog is being created.
    \param qtDialog The Qt widget that will be shown in the dialog.
    \param title The title of the rollup window to be created.
    \param rollFlags Rollout flags, such as APPENDROLL_CLOSED.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The ParamDlg which was created. 
    \sa \ref parammap_qt*/
PB2Export IAutoRendererParamDlg* CreateAutoRenderParamDlg(
    MapID map_id, 
    IRendParams *ip,
    Renderer& renderer,
    IParamBlock2* pb, 
    ClassDesc2* cd, 
    MaxSDK::QMaxParamBlockWidget& qtDialog,
    const MCHAR* title, 
    int rollFlags,
    int category = ROLLUP_CAT_STANDARD);

//! Creates a Qt-based ParamDlg, for use in the render settings panel, which automatically manages the creation and lifetime of the underlying parameter map.
/*! 
    In most cases, this method shouldn't be used directly by plugin developers. Instead, the plugin should call ClassDesc2::CreateParamDlgs()
    to automatically create the ParamDlg for all dialogs flagged with P_AUTO_UI or P_AUTO_UI_QT. But if the plugin needs to manage (perhaps conditionally)
    the creation of its dialogs, then it may call this function directly for individual parameter maps.

    \param map_id The ID of the parameter map for which to create the dialog.
    \param ip The interface passed to Renderer::CreateParamDialog().
    \param renderer The object for which the dialog is being created.
    \param pb The parameter block to which the parameter map is associated.
    \param cd The class descriptor of the object for which the dialog is being created.
    \param qtDialog The Qt widget that will be shown in the dialog.
    \param tabID Identifier for the tab in which the rollup is to be added. This is the ID used to create new tabs
        in implementations of ITabDialogObject::AddTabToDialog().
    \param title The title of the rollup window to be created.
    \param rollFlags Rollout flags, such as APPENDROLL_CLOSED.
    \param category A value controlling the order in which rollouts are positioned. 
    \return The ParamDlg which was created. 
    \sa \ref parammap_qt*/
PB2Export IAutoRendererParamDlg* CreateAutoRenderParamDlg(
	MapID map_id,
	IRendParams *ip,
	Renderer& renderer,
	IParamBlock2* pb,
	ClassDesc2* cd,
	MaxSDK::QMaxParamBlockWidget& qtDialog,
	const Class_ID& tabID,
	const MCHAR* title,
	int rollFlags,
	int category = ROLLUP_CAT_STANDARD);
