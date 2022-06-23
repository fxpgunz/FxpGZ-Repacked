/**********************************************************************
*<
FILE: Zoom.cpp

DESCRIPTION:	

CREATED BY: 

HISTORY: 

*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/
#include "zoom.h"
#include "MouseCursors.h"

//==============================================================================
// Zoom View Mouse Proc
//==============================================================================
ZoomViewMouseProc::ZoomViewMouseProc()
: mViewport(NULL), mCursor(NULL), mOldCursor(NULL), mStarted(false)
{
}

void ZoomViewMouseProc::SetCursor(HCURSOR hCursor)
{
	mCursor = hCursor;
}

void ZoomViewMouseProc::Idle()
{
	if (mStarted)
	{
		if (mViewport)
		{
			SetCursor(mCursor);
			ShowCursor(TRUE);

			mViewport->Invalidate();
			Interface10* ip = GetCOREInterface10();
			ip->RedrawViews(ip->GetTime(), VP_END_SEQUENCE);

			mViewport->UndoAccept();

			mViewport = NULL;				
		}
		mStarted = false;
	}
}

bool ZoomViewMouseProc::NormalStart(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	DbgAssert(!mStarted);

	Interface10* ip = GetCOREInterface10();
	ip->SetActiveViewport(hwnd);
		
	ViewExp& viewExp = ip->GetViewExp(hwnd);		
	if (viewExp.IsAlive())
		mViewport = reinterpret_cast<ViewExp10*>(viewExp.Execute(ViewExp::kEXECUTE_GET_VIEWEXP_10));
	if (NULL == mViewport)
		return false;
	
	SetCursor(mCursor);
	
	mViewport->UndoBegin();

	mViewport->GetAffineTM(mOldViewportMatrix);
	mOldScreenPt  = screenPt;

	mStarted = true;
	ip->RedrawViews(ip->GetTime(), REDRAW_BEGIN); 

	return mStarted;
}

bool ZoomViewMouseProc::OnMousePoint(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	if (0 == point) 
	{		
		// First click in the window
		DbgAssert(!mStarted);
		NormalStart(hwnd, point, flag, screenPt);
	} 
	else 
	{
		Idle();
	}			

	return mStarted;
}

bool ZoomViewMouseProc::OnMouseMove(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	DbgAssert(mStarted);
	if ( ! mViewport || ! mViewport->IsAlive() )
	{
		DbgAssert(!_T("mViewport is not valid!"));
		return false;
	}
	
	SetCursor(mCursor);
	//mOldScreenPt = screenPt;
	IPoint2 panVector = mOldScreenPt - IPoint2(mViewport->getGW()->getWinSizeX()/2, mViewport->getGW()->getWinSizeY()/2);
	int length = Length(panVector);
	float zoom;

	if (panVector.x < 0 || panVector.y < 0)
	{
		zoom = 100.0f - ((float)length * 5.0f / (float)(mViewport->getGW()->getWinSizeX()/2 + mViewport->getGW()->getWinSizeY()/2));
		max(zoom, 0.01f);
	}
	else
	{
		zoom = 100.0f + ((float)length * 5.0f / (float)(mViewport->getGW()->getWinSizeX()/2 + mViewport->getGW()->getWinSizeY()/2));
	}

	mViewport->Zoom( zoom / 100.0f );
	
	mViewport->Invalidate();			
	Interface10* ip = GetCOREInterface10();
	ip->RedrawViews(ip->GetTime(), REDRAW_INTERACTIVE); 

	mOldScreenPt = screenPt;			

	return mStarted;
}

bool ZoomViewMouseProc::OnKeyboard(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	DbgAssert(mStarted);

	if (flag != VK_ESCAPE || point < 0 ) 
		return mStarted;
	return OnMouseAbort(hwnd, point, flag, screenPt);
}

bool ZoomViewMouseProc::OnMouseFreeMove(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	SetCursor(mCursor);
	return mStarted;
}

bool ZoomViewMouseProc::OnMouseAbort(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	DbgAssert(mStarted);
	if ( ! mViewport || ! mViewport->IsAlive() )
	{
		DbgAssert(!_T("mViewport is not valid!"));
		return false;
	}

	mViewport->SetAffineTM(mOldViewportMatrix);
	mViewport->Invalidate();			

	Interface10* ip = GetCOREInterface10();
	ip->RedrawViews(ip->GetTime(), REDRAW_END); 

	mViewport = NULL;
	mStarted = false;

	return mStarted;
}

int ZoomViewMouseProc::proc( 
	HWND hwnd, // handle of window to rotate
	int msg, // mouse event
	int point, // mouse click number
	int flag, // mouse button state
	IPoint2 screenPt ) // screen point clicked 
{
	switch ( msg  ) 
	{
		case MOUSE_POINT:
			return OnMousePoint(hwnd, point, flag, screenPt);

		case MOUSE_FREEMOVE:
			return OnMouseFreeMove(hwnd, point, flag, screenPt);

		case MOUSE_MOVE:
			return OnMouseMove(hwnd, point, flag, screenPt);

		case MOUSE_KEYBOARD:
			return OnKeyboard(hwnd, point, flag, screenPt);

		case MOUSE_ABORT:
			return OnMouseAbort(hwnd, point, flag, screenPt);
	}

	return (mStarted != false);
}

//==============================================================================
// Zoom View Command Mode
//==============================================================================
ZoomViewCommandMode& ZoomViewCommandMode::GetInstance()
{
	static ZoomViewCommandMode theZoomViewCommandMode;
	return theZoomViewCommandMode;
}

MouseCallBack* ZoomViewCommandMode::MouseProc(int *numPoints) 
{ 
	*numPoints = 2; 
	return &proc; 
}

void ZoomViewCommandMode::EnterMode() 
{ 
	// Load and set mouse cursor
	if (NULL == mCursor)
	{
		mCursor = UI::MouseCursors::LoadMouseCursor(UI::MouseCursors::PanHandGrab);
		mOldCursor = SetCursor(mCursor);
		ShowCursor(TRUE);
	}
	proc.SetCursor(mCursor);

	// Display command mode name on status panel
	Interface10* ip = GetCOREInterface10();
	ip->PushPrompt(GetString(IDS_ZOOM_MODE));
}

void ZoomViewCommandMode::ExitMode() 
{ 
	// Restore mouse cursor
	SetCursor(mOldCursor);
	ShowCursor(TRUE);
	DestroyCursor(mCursor);
	mCursor = NULL;
	proc.SetCursor(mCursor);

	// Remove command mode name from status panel
	Interface10* ip = GetCOREInterface10();
	ip->PopPrompt();
}
