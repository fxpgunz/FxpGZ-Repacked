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

#pragma warning(push)
#pragma warning(disable:4100)

class ReferenceTarget;

// Every MtlBase sub-class defines a ParamDlg to manage its part of
// the material editor.
/*! \sa  Class Mtl, Class Texmap, ~{ Materials, Textures and Maps }~.\n\n
\par Description:
Every MtlBase sub-class defines a ParamDlg to manage its part of the material
editor.  */
class ParamDlg : public InterfaceServer 
{
	 public:
			/*! \remarks Returns the unique Class_ID of the plug-in this dialog manages. See
			Class Class_ID. */
			virtual Class_ID ClassID()=0;
			/*! \remarks This sets the current material or texture being edited to the material or
			texture passed.
			\par Parameters:
			<b>ReferenceTarget *m</b>\n\n
			The material or texture to set as current. */
			virtual void SetThing(ReferenceTarget *m)=0;
			/*! \remarks This method returns the current material or texture being edited. */
			virtual ReferenceTarget* GetThing()=0;
			/*! \remarks This method is called when the current time has changed. This gives the
			developer an opportunity to update any user interface data that may need
			adjusting due to the change in time.
			\par Parameters:
			<b>TimeValue t</b>\n\n
			The new current time. */
			virtual void SetTime(TimeValue t)=0;
			/*! \remarks This method should stuff values into all the parameter dialog's controls,
			edit fields etc. In fact this method is now only called after doing a
			"Reset". It is also useful inside the material/map when a
			<b>NotifyRefChanged()</b> is processed it may need to be called. */
			virtual  void ReloadDialog()=0;
			/*! \remarks This method is called by the system to delete an instance of this class.
			A developer must use the same memory manager to delete the item as they
			did to allocate it (in <b>MtlBase::CreateParamDlg()</b>). For example if
			a developer used the <b>new</b> operator to allocate the memory, this
			method would be implemented as <b>{ delete this; }</b> */
			virtual void DeleteThis()=0;
			/*! \remarks This method is called when the dialog box becomes active or inactive.
			Currently this is used when working with color swatch custom controls.
			Color swatches need to know when the dialog box becomes active or
			inactive because they change their method of drawing themselves. When the
			dialog is active, color swatches are drawn in pure color (this requires a
			color register in the palette). When the dialog becomes inactive the
			color swatches are drawn using dithered painting. This is needed because
			there are only 8 available color registers.\n\n
			A method of the color swatch control is called to indicate if it is in an
			active or inactive dialog. See the sample code below. Also see
			Class IColorSwatch.
			\par Parameters:
			<b>BOOL onOff</b>\n\n
			TRUE if the dialog is active; otherwise FALSE.
			\par Sample Code:
			\code
			void NoiseDlg::ActivateDlg(BOOL onOff)
			{
				for (int i=0; i<NCOLS; i++)
					cs[i]->Activate(onOff);
			}
			\endcode */
			virtual void ActivateDlg(BOOL onOff)=0;
			// These need to be implemented if using a TexDADMgr or a MtlDADMgr
			/*! \remarks			This method needs to be implemented if the plug-in texmap is using a
			<b>TexDADMgr</b>. It should return the index of the sub-texmap
			corresponding to the window whose handle is passed. If the handle is not
			valid return -1.
			\par Parameters:
			<b>HWND hwnd</b>\n\n
			The window handle to check.
			\par Default Implementation:
			<b>{ return -1;}</b>
			\par Sample Code:
			\code
			int CellTexParamDlg::FindSubTexFromHWND(HWND hw)
			{
				if (hw==GetDlgItem(pmap->GetHWnd(),IDC_CELLTEX_CELLCOLOR_MAP))
					return 0;
				if (hw==GetDlgItem(pmap->GetHWnd(),IDC_CELLTEX_DIVCOL1_MAP))
					return 1;
				if (hw==GetDlgItem(pmap->GetHWnd(),IDC_CELLTEX_DIVCOL2_MAP))
					return 2;
				return -1;
			}
			\endcode */
			virtual int FindSubTexFromHWND(HWND hwnd){ return -1;} 
			/*! \remarks			This method needs to be implemented if the plug-in material is using a
			<b>MtlDADMgr</b>. It should return the index of the sub-map corresponding
			to the window whose handle is passed. If the handle is not valid return
			-1.
			\par Parameters:
			<b>HWND hwnd</b>\n\n
			The window handle to check.
			\par Default Implementation:
			<b>{ return -1;}</b> */
			virtual int FindSubMtlFromHWND(HWND hwnd){ return -1;} 
			// Generic expansion function
			virtual INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0; } 

};

#pragma warning(pop)
