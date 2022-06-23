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

#include "../RollupConstants.h"
#include "../MaxHeap.h"
#include "../BaseInterface.h"
#include <windows.h>

#pragma warning(push)
#pragma warning(disable:4100)

class QWidget;
class TimeChangeCallback;
class IRollupWindow;
class INode;

// passed to SetPickMode. This is a callback that gets called as
// the user tries to pick objects in the scene.
/*! \sa  Class IMtlParams, Class INode.\n\n
\par Description:
This is the callback object passed to <b>IMtlParams::SetPickMode()</b>. It gets
set so the user can pick objects in the scene. Its methods allow for filtering
the picks, changing cursors over valid hits, and allowing multiple picks.\n\n
All methods of this class are virtual.  */
class PickObjectProc: public MaxHeapOperators {
	 public:
			/*! \remarks Destructor. */
			virtual ~PickObjectProc() {;}
			// Called when the user picks something.
			// return TRUE to end the pick mode.
			/*! \remarks This method is called when the user picks something.
			\par Parameters:
			<b>INode *node</b>\n\n
			This is the node that was selected.
			\return  Return TRUE to end the pick mode; FALSE to ignore the pick and
			to continue to allow the user to pick. */
			virtual BOOL Pick(INode *node)=0;

			// Return TRUE if this is an acceptable hit, FALSE otherwise.
			/*! \remarks This method is called to allow the callback to filter hits.
			It should return TRUE if this is an acceptable hit; FALSE otherwise.
			\par Parameters:
			<b>INode *node</b>\n\n
			This is the node that was selected. */
			virtual BOOL Filter(INode *node)=0;

			// These are called as the mode is entered and exited
			/*! \remarks This method is called when the pick mode has been entered.
			The developer may provide any pre-processing here.
			\par Default Implementation:
			<b>{}</b> */
			virtual void EnterMode() {}
			/*! \remarks This method is called when the pick mode has been exited.
			The developer may provide any post-processing here.
			\par Default Implementation:
			<b>{}</b> */
			virtual void ExitMode() {}

			// Provides two cursor, 1 when over a pickable object and 1 when not.
			/*! \remarks This method is called to get the default (non-hit) cursor
			to use.
			\return  The handle of the non-pickable object cursor or NULL if the
			default cursor should be used.
			\par Default Implementation:
			<b>{return NULL;}</b> */
			virtual HCURSOR GetDefCursor() {return NULL;}
			/*! \remarks This method is called to get the pickable object cursor to
			use.
			\return  The handle of the pickable object cursor or NULL if the default
			cursor should be used.
			\par Default Implementation:
			<b>{return NULL;}</b> */
			virtual HCURSOR GetHitCursor() {return NULL;}

			// Return TRUE to allow the user to pick more than one thing.
			// In this case the Pick method may be called more than once.
			/*! \remarks This method determines if multiple objects can be picked.
			Returning TRUE allows the user to pick more than one thing. In this case
			the <b>Pick()</b> method may be called more than once.
			\par Default Implementation:
			<b>{return FALSE;}</b> */
			virtual BOOL AllowMultiSelect() {return FALSE;}
	 };


// Interface that is passed in to the Mtl or Texture Map when it is in the mtl
// editor.
/*! \sa  Class Mtl, ~{ Materials, Textures and Maps }~, Class IRollupWindow.\n\n
\par Description:
This is the interface that is passed in to the material or texture map when it
is in the material editor. All methods of this class are implemented by the
system.  */
class IMtlParams : public InterfaceServer {
	 public:
	 // call after mouse up's in mtls params
	 // It causes the viewports to be redrawn.
	 /*! \remarks This method may be called to causes the viewports to be
	 redrawn. It should be called when any parameter that affects the look of the
	 material in the viewport has been altered. If the material is not on a
	 visible node in a shaded view, nothing will happen. This method should NOT
	 be called as a spinner is being dragged, but only upon release of the mouse
	 button. */
	 virtual void MtlChanged()=0;  

	 // Adds rollup pages to the Material Params. Returns the window
	 // handle of the dialog that makes up the page.
	 /*! \remarks This method may be called to add a rollup page to the material
	 editor dialog. It returns the window handle of the dialog that makes up the
	 rollup page.
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
	 The following flag is defined:\n\n
	 <b>APPENDROLL_CLOSED</b>\n\n
	 Starts the page in the rolled up state.\n\n
	 <b>int category = ROLLUP_CAT_STANDARD</b>\n\n
	 The category parameter provides flexibility with regard to where a
	 particular rollup should be displayed in the UI. RollupPanels with lower
	 category fields will be displayed before RollupPanels with higher category
	 fields. For RollupPanels with equal category value the one that was added
	 first will be displayed first. Although it is possible to pass any int
	 value as category there exist currently 5 different category defines:
	 <b>ROLLUP_CAT_SYSTEM</b>, <b>ROLLUP_CAT_STANDARD</b>, and
	 <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
	 When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
	 category sticky, meaning it will not read the category from the
	 <b>RollupOrder.cfg</b> file, but rather save the category field that was
	 passed as argument in the <b>CatRegistry</b> and in the
	 <b>RollupOrder.cfg</b> file.\n\n
	 The method will take the category of the replaced rollup in case the flags
	 argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
	 this system works with param maps as well.
	 \return  The window handle of the rollup page. */
	 virtual HWND AddRollupPage( HINSTANCE hInst, const MCHAR *dlgTemplate, 
			DLGPROC dlgProc, const MCHAR *title, LPARAM param=0,DWORD flags=0, int category = ROLLUP_CAT_STANDARD )=0;

	 /*! \remarks	 This method may be called to add a rollup page to the material editor
	 dialog. It returns the window handle of the dialog that makes up the rollup
	 page. This method is currently not used.
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
	 The following flag is defined:\n\n
	 <b>APPENDROLL_CLOSED</b>\n\n
	 Starts the page in the rolled up state.\n\n
	 <b>int category = ROLLUP_CAT_STANDARD</b>\n\n
	 The category parameter provides flexibility with regard to where a
	 particular rollup should be displayed in the UI. RollupPanels with lower
	 category fields will be displayed before RollupPanels with higher category
	 fields. For RollupPanels with equal category value the one that was added
	 first will be displayed first. Although it is possible to pass any int
	 value as category there exist currently 5 different category defines:
	 <b>ROLLUP_CAT_SYSTEM</b>, <b>ROLLUP_CAT_STANDARD</b>, and
	 <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
	 When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
	 category sticky, meaning it will not read the category from the
	 <b>RollupOrder.cfg</b> file, but rather save the category field that was
	 passed as argument in the <b>CatRegistry</b> and in the
	 <b>RollupOrder.cfg</b> file.\n\n
	 The method will take the category of the replaced rollup in case the flags
	 argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
	 this system works with param maps as well.
	 \return  The window handle of the rollup page. */
	 virtual HWND AddRollupPage( HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, 
			DLGPROC dlgProc, const MCHAR *title, LPARAM param=0,DWORD flags=0, int category = ROLLUP_CAT_STANDARD )=0;

    /*! Adds a rollup page that hosts a Qt dialog.
        \param qtWidget the Qt widget that will be hosted in the rollup. <b>This QWidget becomes the ownership of the system</b>; it will be deleted
            by the system once no longer needed.
        \param title The title string, displayed at the top of the rollup.l
        \param rollupFlags Optional rollup flags, such as APPENDROLL_CLOSED, DONTAUTOCLOSE, ROLLUP_SAVECAT, etc.
        \param category The category controls the ordering of the rollups, with lower values being inserted before rollups of higher category. 
        \sa void DeleteRollupPage(QWidget& qtWidget) */
     virtual void AddRollupPage(QWidget& qtWidget, const MCHAR *title, DWORD rollupFlags = 0, int category = ROLLUP_CAT_STANDARD) = 0;

	 // Adds rollup pages to the Material Params. Returns the window
	 // handle of the dialog that makes up the page.
	 /*! \remarks	 This method is used to replace the rollup page whose index is passed.
	 \par Parameters:
	 <b>HWND hOldRollup</b>\n\n
	 The handle to the rollup to replace.\n\n
	 <b>HINSTANCE hInst</b>\n\n
	 The DLL instance handle of the plug-in.\n\n
	 <b>MCHAR *dlgTemplate</b>\n\n
	 The dialog template for the rollup page.\n\n
	 <b>DLGPROC dlgProc</b>\n\n
	 The dialog proc to handle the message sent to the rollup page.\n\n
	 <b>MCHAR *title</b>\n\n
	 The title displayed in the title bar.\n\n
	 <b>LPARAM param=0</b>\n\n
	 Any specific data to pass along may be stored here.\n\n
	 <b>DWORD flags=0</b>\n\n
	 Append rollup page flags:\n\n
	 <b>APPENDROLL_CLOSED</b>\n\n
	 Starts the page in the rolled up state.\n\n
	 <b>int category = ROLLUP_CAT_STANDARD</b>\n\n
	 The category parameter provides flexibility with regard to where a
	 particular rollup should be displayed in the UI. RollupPanels with lower
	 category fields will be displayed before RollupPanels with higher category
	 fields. For RollupPanels with equal category value the one that was added
	 first will be displayed first. Although it is possible to pass any int
	 value as category there exist currently 5 different category defines:
	 <b>ROLLUP_CAT_SYSTEM</b>, <b>ROLLUP_CAT_STANDARD</b>, and
	 <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
	 When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
	 category sticky, meaning it will not read the category from the
	 <b>RollupOrder.cfg</b> file, but rather save the category field that was
	 passed as argument in the <b>CatRegistry</b> and in the
	 <b>RollupOrder.cfg</b> file.\n\n
	 The method will take the category of the replaced rollup in case the flags
	 argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
	 this system works with param maps as well.
	 \return  The handle of the replacement page is returned. */
	 virtual HWND ReplaceRollupPage( HWND hOldRollup, HINSTANCE hInst, const MCHAR *dlgTemplate, 
			DLGPROC dlgProc, const MCHAR *title, LPARAM param=0,DWORD flags=0, int category = ROLLUP_CAT_STANDARD  )=0;

	 /*! \remarks	 This method is used to replace the rollup page whose index is passed. This
	 method is currently not being used.
	 \par Parameters:
	 <b>HWND hOldRollup</b>\n\n
	 The handle to the rollup to replace.\n\n
	 <b>HINSTANCE hInst</b>\n\n
	 The DLL instance handle of the plug-in.\n\n
	 <b>DLGTEMPLATE *dlgTemplate</b>\n\n
	 The dialog template for the rollup page.\n\n
	 <b>DLGPROC dlgProc</b>\n\n
	 The dialog proc to handle the message sent to the rollup page.\n\n
	 <b>MCHAR *title</b>\n\n
	 The title displayed in the title bar.\n\n
	 <b>LPARAM param=0</b>\n\n
	 Any specific data to pass along may be stored here.\n\n
	 <b>DWORD flags=0</b>\n\n
	 Append rollup page flags:\n\n
	 <b>APPENDROLL_CLOSED</b>\n\n
	 Starts the page in the rolled up state.\n\n
	 <b>int category = ROLLUP_CAT_STANDARD</b>\n\n
	 The category parameter provides flexibility with regard to where a
	 particular rollup should be displayed in the UI. RollupPanels with lower
	 category fields will be displayed before RollupPanels with higher category
	 fields. For RollupPanels with equal category value the one that was added
	 first will be displayed first. Although it is possible to pass any int
	 value as category there exist currently 5 different category defines:
	 <b>ROLLUP_CAT_SYSTEM</b>, <b>ROLLUP_CAT_STANDARD</b>, and
	 <b>ROLLUP_CAT_CUSTATTRIB</b>.\n\n
	 When using <b>ROLLUP_SAVECAT</b>, the rollup page will make the provided
	 category sticky, meaning it will not read the category from the
	 <b>RollupOrder.cfg</b> file, but rather save the category field that was
	 passed as argument in the <b>CatRegistry</b> and in the
	 <b>RollupOrder.cfg</b> file.\n\n
	 The method will take the category of the replaced rollup in case the flags
	 argument contains <b>ROLLUP_USEREPLACEDCAT</b>. This is mainly done, so that
	 this system works with param maps as well.
	 \return  The handle of the replacement page is returned. */
	 virtual HWND ReplaceRollupPage( HWND hOldRollup, HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, 
			DLGPROC dlgProc, const MCHAR *title, LPARAM param=0,DWORD flags=0, int category = ROLLUP_CAT_STANDARD  )=0;

	 // Removes a rollup page and destroys it.  When a dialog is destroyed
	 // it need not delete all its rollup pages: the Mtl Editor will do
	 // this for it, and it is more efficient.
	 /*! \remarks This method may be called to remove a rollup page and destroy
	 it.
	 \par Parameters:
	 <b>HWND hRollup</b>\n\n
	 The handle of the rollup window. This is the handle returned from
	 <b>AddRollupPage()</b>. */
	 virtual void DeleteRollupPage( HWND hRollup )=0;

    /*! Removes a rollup page that hosts a Qt dialog.
        \remark The QWidget will get destroyed as a result of calling this.
        \sa void AddRollupPage(QWidget& qtWidget, const MCHAR *title, DWORD rollupFlags, int category) */
     virtual void DeleteRollupPage(QWidget& qtWidget) = 0;

	 // When the user mouses down in dead area, the plug-in should pass
	 // mouse messages to this function which will pass them on to the rollup.
	 /*! \remarks This method allows hand cursor scrolling when the user clicks
	 the mouse in an unused area of the dialog. When the user mouses down in dead
	 area of the material editor, the plug-in should pass mouse messages to this
	 function.\n\n
	 Note: In 3ds Max 2.0 and later only use of this method is no longer required
	 -- the functionality happens automatically.
	 \par Parameters:
	 <b>HWND hDlg</b>\n\n
	 The window handle of the dialog.\n\n
	 <b>UINT message</b>\n\n
	 The message sent to the dialog proc.\n\n
	 <b>WPARAM wParam</b>\n\n
	 Passed in to the dialog proc. Pass along to this method.\n\n
	 <b>LPARAM lParam</b>\n\n
	 Passed in to the dialog proc. Pass along to this method.
	 \par Example:
	 <b>case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_MOUSEMOVE:</b>\n\n
	 <b>im-\>iMtlParams-\>RollupMouseMessage(hDlg,message,wParam,lParam);</b> */
	 virtual void RollupMouseMessage( HWND hDlg, UINT message, 
						WPARAM wParam, LPARAM lParam )=0;

	 /*! \remarks Returns nonzero if the rollup page whose handle is passed is
	 open; otherwise zero.
	 \par Parameters:
	 <b>HWND hwnd</b>\n\n
	 The window handle of the rollup page to check. */
	 virtual int IsRollupPanelOpen(HWND hwnd)=0;
	 /*! \remarks Returns the rollup scroll position. This is used, for example,
	 by the Standard material because it saves and restores the rollup page
	 positions with the material. This is just a convenience for the user. */
	 virtual int GetRollupScrollPos()=0;
	 /*! \remarks This method may be called to set the rollup scroll position.
	 If the position was previously saved, this method may be called to restore
	 it. The Standard material uses this method because it saves and restores the
	 rollup positions with the material as a convenience for the user.
	 \par Parameters:
	 <b>int spos</b>\n\n
	 Specifies the rollup scroll position. */
	 virtual void SetRollupScrollPos(int spos)=0;

	 /*! \remarks	 Registers a callback object that will get called every 
	 time the current animation time is changed.
	 \par Parameters:
	 <b>TimeChangeCallback *tc</b>\n\n
	 The callback called when the time changes. See
	 Class TimeChangeCallback. */
	 virtual void RegisterTimeChangeCallback(TimeChangeCallback *tc)=0;
	 /*! \remarks	 This method un-registers the time change callback.
	 \par Parameters:
	 <b>TimeChangeCallback *tc</b>\n\n
	 The callback called when the time changes. See
	 Class TimeChangeCallback. */
	 virtual void UnRegisterTimeChangeCallback(TimeChangeCallback *tc)=0;

	 // Registers a dialog window so IsDlgMesage() gets called for it.
	 // This is done automatically for Rollup Pages.
	 /*! \remarks This is called automatically in <b>AddRollupPage()</b>, so a
	 plug-in doesn't need to do it. It is now obsolete.
	 \par Parameters:
	 <b>HWND hDlg</b>\n\n
	 The handle of the dialog window. */
	 virtual void RegisterDlgWnd( HWND hDlg )=0;
	 /*! \remarks This method is called automatically and is now obsolete.
	 \par Parameters:
	 <b>HWND hDlg</b>\n\n
	 The handle of the dialog window. */
	 virtual int UnRegisterDlgWnd( HWND hDlg )=0;

	 // get the current time.
	 /*! \remarks Returns the current time (the frame slider position).
	 \return  The current time.\n\n
	 Pick an object from the scene */
	 virtual TimeValue GetTime()=0;   

	 // Pick an object from the scene
	 /*! \remarks	 Calling this methods puts the user into a pick mode where they can select
	 items in the scene.
	 \par Parameters:
	 <b>PickObjectProc *proc</b>\n\n
	 This is the callback object for the picking. Its methods allow for filtering
	 the picks, changing cursors over valid hits, and allowing multiple picks.
	 See Class PickObjectProc. */
	 virtual void SetPickMode(PickObjectProc *proc)=0;
	 /*! \remarks	 This method is called to terminate the pick mode. */
	 virtual void EndPickMode()=0;

	 // JBW 10/19/98: get interface to mtl editor rollup
	 /*! \remarks	 This method returns an interface to Materials Editor rollup. */
	 virtual IRollupWindow *GetMtlEditorRollup()=0;

	 // Generic expansion function
	 virtual INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0; } 
	 };

#pragma warning(pop)
