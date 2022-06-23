/**********************************************************************
 *<
	FILE: ViewportNavigator.cpp

	DESCRIPTION:	Global Utility Plugin that allows the user to start
	viewport manipulation\navigation command modes such as Pan, Zoom, Rotate.

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2007, All Rights Reserved.
 **********************************************************************/

#include "ViewportNavigator.h"
#include "pan.h"
#include "rotate.h"
#include "zoom.h"

const Class_ID ViewportNavigator::mClassID = Class_ID(0x831b0af4, 0x8c02bcc1);

ViewportNavigator& ViewportNavigator::GetInstance()
{
	static ViewportNavigator theViewportNavigator;
	return theViewportNavigator;
}

Class_ID ViewportNavigator::GetClassID()
{
	return mClassID;
}

ViewportNavigator::ViewportNavigator()
: mpIUtil(NULL), mpInterface(NULL), mDlgWnd(NULL)
{
}

ViewportNavigator::~ViewportNavigator()
{
	// Nothing to do
}

void ViewportNavigator::DeleteThis()
{
	delete this;
}

void ViewportNavigator::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->mpIUtil = iu;
	this->mpInterface = ip;
	mDlgWnd = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		ViewportNavigator::DlgProc,
		GetString(IDS_PARAMS),
		(LPARAM)this);
}

void ViewportNavigator::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->mpIUtil = NULL;
	this->mpInterface = NULL;
	ip->DeleteRollupPage(mDlgWnd);
	mDlgWnd = NULL;
}

void ViewportNavigator::Init(HWND hWnd)
{
	Interface10* ip = GetCOREInterface10();
	ip->RegisterCommandModeChangedCallback(&mUIUpdater);
}

void ViewportNavigator::Destroy(HWND hWnd)
{
	Interface10* ip = GetCOREInterface10();
	ip->UnRegisterCommandModeChangedCallback(&mUIUpdater);
}

//------------------------------------------------------------------------------
// User Interface
//------------------------------------------------------------------------------
INT_PTR CALLBACK ViewportNavigator::DlgProc(
	HWND hWnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
			ViewportNavigator::GetInstance().Init(hWnd);
			break;
/*
		case WM_SETCURSOR: 
			// DO we needs this?
			// If the window is minimized, draw the hCurs3 cursor. 
			// If the window is not minimized, draw the default cursor (class cursor). 
			
			if (IsIconic(hwnd)) 
			{ 
				SetCursor(hCurs3); 
				break; 
			} 
		break;
*/
		case WM_DESTROY:
			ViewportNavigator::GetInstance().Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDC_PAN:
				{				
					// Toggle Pan command mode
					Interface* ip = GetCOREInterface();
					PanViewCommandMode* pan = &PanViewCommandMode::GetInstance();
					if (ip->GetCommandMode() == pan)
						ip->DeleteMode(pan);
					else 
						ip->PushCommandMode(pan);
				}
				break;

				case IDC_ROTATE:
				{
					// Toggle Rotate command mode
					Interface* ip = GetCOREInterface();
					RotateViewCommandMode* rotate = &RotateViewCommandMode::GetInstance();
					if (ip->GetCommandMode() == rotate)
						ip->DeleteMode(rotate);
					else 
						ip->PushCommandMode(rotate);
				}
				break;

				case IDC_ZOOM:
				{
					// Toggle Zoom command mode
					Interface* ip = GetCOREInterface();
					ZoomViewCommandMode* zoom = &ZoomViewCommandMode::GetInstance();
					if (ip->GetCommandMode() == zoom)
						ip->DeleteMode(zoom);
					else 
						ip->PushCommandMode(zoom);
				}
				break;
			}
			break;


		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			GetCOREInterface()->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return 0;
	}
	return 1;
}

void ViewportNavigator::UpdateUI(HWND hDlgWnd, UINT btnID, bool btnActive)
{
	UINT bBtnChecked = (btnActive ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hDlgWnd, btnID, bBtnChecked);
}

void ViewportNavigator::UIUpdater::ModeChanged(CommandMode *oldM, CommandMode *newM)
{
	Interface* ip = GetCOREInterface();

	bool bPanActive = (ip->GetCommandMode() == &PanViewCommandMode::GetInstance());
	bool bRotateActive = (ip->GetCommandMode() == &RotateViewCommandMode::GetInstance());
	bool bZoomActive = (ip->GetCommandMode() == &ZoomViewCommandMode::GetInstance());
	
	ViewportNavigator::UpdateUI(ViewportNavigator::GetInstance().GetDlgHWnd(), IDC_PAN, bPanActive);
	ViewportNavigator::UpdateUI(ViewportNavigator::GetInstance().GetDlgHWnd(), IDC_ROTATE, bRotateActive);
	ViewportNavigator::UpdateUI(ViewportNavigator::GetInstance().GetDlgHWnd(), IDC_ZOOM, bZoomActive);
}



