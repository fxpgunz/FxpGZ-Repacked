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
#include "../maxtypes.h"
#include "./RollupConstants.h"

class MtlBase;
class TimeChangeCallback;
class QWidget;
class RendPickProc;
class IRollupWindow;
class ITabbedDialog;
class ITabPage;

/** \defgroup Material_Browser_Flags Material Browser Flags 
 *Flag bits for DoMaterialBrowseDlg() */
//@{
#define BROWSE_MATSONLY		(1<<0)	//!< Materials only
#define BROWSE_MAPSONLY		(1<<1)	//!< Maps only
#define BROWSE_INCNONE		(1<<2) 	//!< Include 'None' as an option
#define BROWSE_INSTANCEONLY	(1<<3) 	//!< Only allow instances, no copy
#define BROWSE_TO_MEDIT_SLOT (1<<4) //!< browsing to medit slot
#define BROWSE_EXISTING_ONLY (1<<5)	//!< Browse for existing materials only; no new materials.
//@}

/*! This is the interface given to a renderer, or atmospheric effect when it needs
to display its parameters. For a renderer, the Render Scene dialog may be
extended using this class. All methods of this class are implemented by the
system.  
 \see  Class TimeChangeCallback,  Class RendPickProc.*/
class IRendParams : public InterfaceServer
{
public:
	/*! Returns the current position of the frame slider. */
	virtual TimeValue GetTime()=0;

	/*! Register a callback object that will get called every time
	the current animation time is changed.
	\par Parameters:
	<b>TimeChangeCallback *tc</b>\n\n
	The callback object to register. */
	virtual void RegisterTimeChangeCallback(TimeChangeCallback *tc)=0;
	/*! Un-registers a callback object registered using
	RegisterTimeChangeCallback().
	\par Parameters:
	<b>TimeChangeCallback *tc</b>\n\n
	The callback object to un-register. */
	virtual void UnRegisterTimeChangeCallback(TimeChangeCallback *tc)=0;

	/*! Brings up the material browse dialog allowing the user to
	select a material.
	\par Parameters:
	<b>HWND hParent</b>\n\n
	The parent window handle.\n\n
	<b>DWORD flags</b>\n\n
	See \ref Material_Browser_Flags .\n\n
	<b>BOOL \&newMat</b>\n\n
	Set to TRUE if the material is new OR cloned; otherwise FALSE.\n\n
	<b>BOOL \&cancel</b>\n\n
	Set to TRUE if the user cancels the dialog; otherwise FALSE.
	\return  The material returned will be NULL if the user selects 'None' */
	virtual MtlBase *DoMaterialBrowseDlg(HWND hParent,DWORD flags,BOOL &newMat,BOOL &cancel)=0;

	/*! This method adds rollup pages to the dialog and returns the
	window handle of the page.
	\par Parameters:
	<b>HINSTANCE hInst</b>\n\n
	The DLL instance handle of the plug-in.\n\n
	<b>MCHAR *dlgTemplate</b>\n\n
	The dialog template for the rollup page.\n\n
	<b>DLGPROC dlgProc</b>\n\n
	The dialog proc to handle the message sent to the rollup page.\n\n
	<b>MCHAR *title</b>\n\n
	The title displayed in the title bar.\n\n
	<b>LPARAM param=0</b>\n\n
	Any specific data to pass along may be stored here. This may be later
	retrieved using the GetWindowLong() call from the Windows API.\n\n
	<b>DWORD flags=0</b>\n\n
	<b>APPENDROLL_CLOSED</b>\n\n
	Starts the page in the rolled up state.\n\n
	<b>int category = ROLLUP_CAT_STANDARD</b>\n\n
	The category parameter provides flexibility with regard to where a
	particular rollup should be displayed in the UI. RollupPanels with lower
	category fields will be displayed before RollupPanels with higher category
	fields. For RollupPanels with equal category value the one that was added
	first will be displayed first. Allthough it is possible to pass any int
	value as category there exist currently 5 different category defines:
	<b>ROLLUP_CAT_SYSTEM</b>, <b>ROLLUP_CAT_STANDARD</b>, and
	<b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
	When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
	category sticky, meaning it will not read the category from the
	<b>RollupOrder.cfg</b> file, but rather save the category field that was
	passed as argument in the <b>CatRegistry</b> and in the
	<b>RollupOrder.cfg</b> file.\n\n
	The method will take the category of the replaced rollup in case the flags
	argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so
	that this system works with param maps as well.
	\return  The window handle of the rollup page. */
	virtual HWND AddRollupPage(
		HINSTANCE hInst, 
		const MCHAR *dlgTemplate, 
		DLGPROC dlgProc, 
		const MCHAR *title, 
		LPARAM param=0, 
		DWORD flags=0, 
		int category = ROLLUP_CAT_STANDARD)=0;

	/*! \remarks	This method adds rollup pages to the dialog and returns the window handle
	of the page. This method is currently not being used.
	\par Parameters:
	<b>HINSTANCE hInst</b>\n\n
	The DLL instance handle of the plug-in.\n\n
	<b>DLGTEMPLATE *dlgTemplate</b>\n\n
	The dialog template for the rollup page.\n\n
	<b>DLGPROC dlgProc</b>\n\n
	The dialog proc to handle the message sent to the rollup page.\n\n
	<b>MCHAR *title</b>\n\n
	The title displayed in the title bar.\n\n
	<b>LPARAM param=0</b>\n\n
	Any specific data to pass along may be stored here. This may be later
	retrieved using the GetWindowLong() call from the Windows API.\n\n
	<b>DWORD flags=0</b>\n\n
	<b>APPENDROLL_CLOSED</b>\n\n
	Starts the page in the rolled up state.\n\n
	<b>int category = ROLLUP_CAT_STANDARD</b>\n\n
	The category parameter provides flexibility with regard to where a
	particular rollup should be displayed in the UI. RollupPanels with lower
	category fields will be displayed before RollupPanels with higher category
	fields. For RollupPanels with equal category value the one that was added
	first will be displayed first. Allthough it is possible to pass any int
	value as category there exist currently 5 different category defines:
	<b>ROLLUP_CAT_SYSTEM</b>, <b>ROLLUP_CAT_STANDARD</b>, and
	<b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
	When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
	category sticky, meaning it will not read the category from the
	<b>RollupOrder.cfg</b> file, but rather save the category field that was
	passed as argument in the <b>CatRegistry</b> and in the
	<b>RollupOrder.cfg</b> file.\n\n
	The method will take the category of the replaced rollup in case the flags
	argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so
	that this system works with param maps as well.
	\return  The window handle of the rollup page. */
	virtual HWND AddRollupPage(
		HINSTANCE hInst, 
		DLGTEMPLATE *dlgTemplate, 
		DLGPROC dlgProc, 
		const MCHAR *title, 
		LPARAM param=0, 
		DWORD flags=0, 
		int category = ROLLUP_CAT_STANDARD)=0;

    /*! Adds a rollup page that hosts a Qt dialog.
        \param qtWidget the Qt widget that will be hosted in the rollup. <b>This QWidget becomes the ownership of the system</b>; it will be deleted
            by the system once no longer needed.
        \param title The title string, displayed at the top of the rollup.l
        \param rollupFlags Optional rollup flags, such as APPENDROLL_CLOSED, DONTAUTOCLOSE, ROLLUP_SAVECAT, etc.
        \param category The category controls the ordering of the rollups, with lower values being inserted before rollups of higher category. 
        \sa void DeleteRollupPage(QWidget& qtWidget) */
    virtual void AddRollupPage( QWidget& qtWidget, const MCHAR *title, DWORD rollupFlags = 0, int category = ROLLUP_CAT_STANDARD ) = 0;

	/*! Removes a rollup page and destroys it.
	\par Parameters:
	<b>HWND hRollup</b>\n\n
	The handle of the rollup window. This is the handle returned from
	AddRollupPage(). */
	virtual void DeleteRollupPage(HWND hRollup)=0;

    /*! Removes a rollup page that hosts a Qt dialog.
        \remark The QWidget will get destroyed as a result of calling this.
        \sa void AddRollupPage(QWidget& qtWidget, const MCHAR *title, DWORD rollupFlags, int category) */
    virtual void DeleteRollupPage(QWidget& qtWidget) = 0;

	/*! This allows hand cursor scrolling when the user clicks the
	mouse in an unused area of the dialog. When the user mouses down in dead
	area of the dialog, the plug-in should pass mouse messages to this function
	which will pass them on to the rollup.\n\n
	Note: In 3ds Max 2.0 and later only use of this method is no longer
	required -- the functionality happens automatically.
	\par Parameters:
	<b>HWND hDlg</b>\n\n
	The window handle of the dialog.\n\n
	<b>UINT message</b>\n\n
	The message sent to the dialog proc.\n\n
	<b>WPARAM wParam</b>\n\n
	Passed in to the dialog proc. Pass along to this method.\n\n
	<b>LPARAM lParam</b>\n\n
	Passed in to the dialog proc. Pass along to this method. */
	virtual void RollupMouseMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)=0;

	/*! This will set the command mode to a standard pick mode. The
	callback implements hit testing and a method that is called when the user
	actually picks an item.
	\par Parameters:
	<b>RendPickProc *proc</b>\n\n
	The callback object. See Class RendPickProc. */
	virtual void SetPickMode(RendPickProc *proc)=0;
	
	/*! If a plug-in is finished editing its parameters it should not
	leave the user in a pick mode. This method will flush out any pick modes in
	the command stack. */
	virtual void EndPickMode()=0;
		
	/*! When a plugin has a Texmap, clicking on the button associated
	with that map should cause this routine to be called.
	\par Parameters:
	<b>MtlBase *mb</b>\n\n
	The <b>MtlBase</b> (<b>Texmap</b> or <b>Mtl</b>) to put to the materials
	editor. */
	virtual void PutMtlToMtlEditor(MtlBase *mb)=0;

	/*! This method is used internally. */
	virtual float GetMaxPixelSize() = 0;

	/*! \remarks	This is a general purpose function that allows the API to be extended in the
	future. The 3ds Max development team can assign new <b>cmd</b> numbers and
	continue to add functionality to this class without having to 'break' the API.
	\par Parameters:
	<b>int cmd</b>\n\n
	The index of the command to execute.\n\n
	<b>ULONG_PTR arg1=0</b>\n\n
	Optional argument 1. See the documentation where the <b>cmd</b> option is
	discussed for more details on these parameters.\n\n
	<b>ULONG_PTR arg2=0</b>\n\n
	Optional argument 2.\n\n
	<b>ULONG_PTR arg3=0</b>\n\n
	Optional argument 3.
	\return  An integer return value. See the documentation where the <b>cmd</b>
	option is discussed for more details on the meaning of this value. */
	virtual INT_PTR Execute(int /*cmd*/, ULONG_PTR /*arg1*/=0, ULONG_PTR /*arg2*/=0, ULONG_PTR /*arg3*/=0) { return 0; } 

	// JBW 12/1/98: get interface to rollup window interface
	virtual IRollupWindow* GetIRollup()=0;

	// Adds rollup pages to the render params dialog. Returns the window
	// handle of the dialog that makes up the page.
	virtual HWND AddTabRollupPage(const Class_ID& /*id*/, HINSTANCE hInst, const MCHAR *dlgTemplate, 
		DLGPROC dlgProc, const MCHAR *title, LPARAM param=0, DWORD flags=0, int category = ROLLUP_CAT_STANDARD)
	{
		return AddRollupPage(hInst, dlgTemplate, dlgProc, title, param, flags, category);
	}

	virtual HWND AddTabRollupPage(const Class_ID& /*id*/, HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, 
		DLGPROC dlgProc, const MCHAR *title, LPARAM param=0, DWORD flags=0, int category = ROLLUP_CAT_STANDARD)
	{
		return AddRollupPage(hInst, dlgTemplate, dlgProc, title, param, flags, category);
	}

	// Removes a rollup page and destroys it.
	virtual void DeleteTabRollupPage(const Class_ID& /*id*/, HWND hRollup)
	{
		DeleteRollupPage(hRollup);
	}

	// When the user mouses down in dead area, the plug-in should pass
	// mouse messages to this function which will pass them on to the rollup.
	virtual void RollupTabMouseMessage(const Class_ID& /*id*/, HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		RollupMouseMessage(hDlg, message, wParam, lParam);
	}

	// JBW 12/1/98: get interface to rollup window interface
	virtual IRollupWindow* GetTabIRollup(const Class_ID& /*id*/)
	{
		return GetIRollup();
	}

	// Return the tabbed dialog for these render params
	virtual ITabbedDialog* GetTabDialog()
	{
		return NULL;
	}

	// Return the page with the given class id.
	virtual ITabPage* GetTabPage(const Class_ID& /*id*/)
	{
		return NULL;
	}
};

//==================================================================================================
// class IRendParams2
//
/*! Extension class that adds functionality to class IRendParams2 for 3ds Max 2017 Service Pack 2.

    A pointer to this extension interface may be retrieved through a dynamic_cast from a IRendParams.

    Note: this class will eventually disappear, having its functionality folded into IRendParams.
*/
class IRendParams2 : public IRendParams 
{
public:

    /*! Adds a rollup page that hosts a Qt dialog, to a specific tab.
        \param qtWidget the Qt widget that will be hosted in the rollup. <b>This QWidget becomes the ownership of the system</b>; it will be deleted
            by the system once no longer needed.
        \param tabID Identifier for the tab in which the rollup is to be added. This is the ID used to create new tabs
            in implementations of ITabDialogObject::AddTabToDialog().
        \param title The title string, displayed at the top of the rollup.l
        \param rollupFlags Optional rollup flags, such as APPENDROLL_CLOSED, DONTAUTOCLOSE, ROLLUP_SAVECAT, etc.
        \param category The category controls the ordering of the rollups, with lower values being inserted before rollups of higher category. 
        \sa void DeleteTabRollupPage(QWidget& qtWidget, const Class_ID& tabID) */
	virtual void AddTabRollupPage(
		QWidget& qtWidget, 
		const Class_ID& tabID,
		const MCHAR *title,
		DWORD rollupFlags = 0, 
		int category = ROLLUP_CAT_STANDARD) = 0;
    /*! Removes a rollup page that hosts a Qt dialog, from a specific tab.
        \param qtWidget the Qt widget to be removed.
        \param tabID Identifier for the tab from which the rollup is to be removed; the same value passed to AddTabRollupPage().
        \remark The QWidget will get destroyed as a result of calling this.     
        \sa void AddTabRollupPage(QWidget& qtWidget, const MCHAR *title, DWORD rollupFlags, int category) */
	virtual void DeleteTabRollupPage(QWidget& qtWidget, const Class_ID& tabID) = 0;
};
