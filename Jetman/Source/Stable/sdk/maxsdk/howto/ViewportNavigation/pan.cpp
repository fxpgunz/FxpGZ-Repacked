/**********************************************************************
*<
FILE: Pan.cpp

DESCRIPTION:	

CREATED BY: 

HISTORY: 

*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/
#include "pan.h"
#include "MouseCursors.h"

//==============================================================================
// Pan View Mouse Proc
//==============================================================================
PanViewMouseProc::PanViewMouseProc()
: mViewport(NULL), mCursor(NULL), mOldCursor(NULL), mStarted(false)
{
}

void PanViewMouseProc::SetCursor(HCURSOR hCursor)
{
	mCursor = hCursor;
}

void PanViewMouseProc::Idle()
{
	if (mStarted)
	{
		if (mViewport)
		{
			SetCursor(mCursor);

			mViewport->Invalidate();
			Interface10* ip = GetCOREInterface10();
			ip->RedrawViews(ip->GetTime(), VP_END_SEQUENCE);

			mViewport->UndoAccept();

			mViewport = NULL;				
		}
		mStarted = false;
	}
}

bool PanViewMouseProc::NormalStart(HWND hwnd, int point, int flag, const IPoint2& screenPt )
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
	mOldScreenPt.x = (float)screenPt.x;
	mOldScreenPt.y = (float)screenPt.y;

	mStarted = true;
	ip->RedrawViews(ip->GetTime(), REDRAW_BEGIN); 

	return mStarted;
}

bool PanViewMouseProc::OnMousePoint(HWND hwnd, int point, int flag, const IPoint2& screenPt )
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

bool PanViewMouseProc::OnMouseMove(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	DbgAssert(mStarted);
	
	SetCursor(mCursor);

	Point2 mCurScreenPt((float)screenPt.x, (float)screenPt.y);

	if (flag & MOUSE_SHIFT) 
	{
		// When Shift is pressed, we constrain the Pan to one direction
		if (abs(screenPt.y - mOldScreenPt.y) > abs(screenPt.x - mOldScreenPt.x))
			// Constrain in X
			mCurScreenPt.x = mOldScreenPt.x;
		else
			// Constrain in Y
			mCurScreenPt.y = mOldScreenPt.y;
	}

	Point2 delta = mCurScreenPt - mOldScreenPt;
	mViewport->Pan(delta);

	mViewport->Invalidate();			
	Interface10* ip = GetCOREInterface10();
	ip->RedrawViews(ip->GetTime(), REDRAW_INTERACTIVE); 

	mOldScreenPt.x = (float)screenPt.x;
	mOldScreenPt.y = (float)screenPt.y;

	return mStarted;
}

bool PanViewMouseProc::OnKeyboard(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	DbgAssert(mStarted);

	if (flag != VK_ESCAPE || point < 0 ) 
		return mStarted;
	return OnMouseAbort(hwnd, point, flag, screenPt);
}

bool PanViewMouseProc::OnMouseFreeMove(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	SetCursor(mCursor);
	return mStarted;
}

bool PanViewMouseProc::OnMouseAbort(HWND hwnd, int point, int flag, const IPoint2& screenPt )
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

int PanViewMouseProc::proc( 
	HWND hwnd, // handle of window to pan
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
// Pan View Command Mode
//==============================================================================
PanViewCommandMode& PanViewCommandMode::GetInstance()
{
	static PanViewCommandMode thePanViewCommandMode;
	return thePanViewCommandMode;
}

MouseCallBack* PanViewCommandMode::MouseProc(int *numPoints) 
{ 
	*numPoints = 2; 
	return &proc; 
}

void PanViewCommandMode::EnterMode() 
{ 
	// Load and set mouse cursor
	if (NULL == mCursor)
	{
		mCursor = UI::MouseCursors::LoadMouseCursor(UI::MouseCursors::PanHand);
		mOldCursor = SetCursor(mCursor);
		ShowCursor(TRUE);
	}
	proc.SetCursor(mCursor);

	// Display command mode name on status panel
	Interface10* ip = GetCOREInterface10();
	ip->PushPrompt(GetString(IDS_PAN_MODE));
}

void PanViewCommandMode::ExitMode() 
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
