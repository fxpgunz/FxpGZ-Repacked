/**********************************************************************
*<
FILE: Rotate.cpp

DESCRIPTION:	

CREATED BY: 

HISTORY: 

*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/
#include "rotate.h"
#include "MouseCursors.h"
//==============================================================================
// Rotate View Mouse Proc
//==============================================================================
RotateViewMouseProc::RotateViewMouseProc()
: mViewport(NULL), mCursor(NULL), mOldCursor(NULL), mStarted(false)
{
}

void RotateViewMouseProc::SetCursor(HCURSOR hCursor)
{
	mCursor = hCursor;
}

void RotateViewMouseProc::Idle()
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

bool RotateViewMouseProc::NormalStart(HWND hwnd, int point, int flag, const IPoint2& screenPt )
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

bool RotateViewMouseProc::OnMousePoint(HWND hwnd, int point, int flag, const IPoint2& screenPt )
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

bool RotateViewMouseProc::OnMouseMove(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	DbgAssert(mStarted);
	if ( ! mViewport || ! mViewport->IsAlive() )
	{
		DbgAssert(!_T("Viewport is not valid!"));
		return false;
	}
	
	SetCursor(mCursor);

	IPoint2 mCurScreenPt = screenPt;
	if (abs(screenPt.y - mOldScreenPt.y) > abs(screenPt.x - mOldScreenPt.x))
		// Constrain in X
		mCurScreenPt.x = mOldScreenPt.x;
	else
		// Constrain in Y
		mCurScreenPt.y = mOldScreenPt.y;
	
	// find out the user is moving the mouse in which direction
	IPoint2 delta = mCurScreenPt - mOldScreenPt;

	Point3 center;
	Matrix3 svmat;
	mViewport->GetAffineTM(svmat);

	if(mViewport->GetViewType() == VIEW_PERSP_USER)
		center = Point3(0.0f, 0.0f, -mViewport->GetFocalDist()) - svmat.GetTrans();
	else
		center = svmat.GetTrans();//mViewport->getOffset();

	float angle = (flag & MOUSE_CTRL) ? 10.0f : 1.0f;	

	if (delta.x < 0) // Going left
	{
		if (flag & MOUSE_SHIFT) 
			mViewport->Rotate( RotateZMatrix(  DegToRad(angle) ), center );
		else
			mViewport->Rotate( Inverse(svmat) * RotateZMatrix(  DegToRad(angle) ) * svmat, center );
	}
	else if (delta.x > 0) // Going Right
	{
		if (flag & MOUSE_SHIFT) 
			mViewport->Rotate( RotateZMatrix( -DegToRad(angle) ), center );
		else
			mViewport->Rotate( Inverse(svmat) * RotateZMatrix( -DegToRad(angle) ) * svmat, center );
	}
	else if (delta.y < 0) // going Up
	{
		mViewport->Rotate( RotateXMatrix(  DegToRad(angle) ), center );
	}
	else // Going down
	{
		mViewport->Rotate( RotateXMatrix( -DegToRad(angle) ), center );
	}

	mViewport->Invalidate();			
	Interface10* ip = GetCOREInterface10();
	ip->RedrawViews(ip->GetTime(), REDRAW_INTERACTIVE); 

	mOldScreenPt = screenPt;			

	return mStarted;
}

bool RotateViewMouseProc::OnKeyboard(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	DbgAssert(mStarted);

	if (flag != VK_ESCAPE || point < 0 ) 
		return mStarted;
	return OnMouseAbort(hwnd, point, flag, screenPt);
}

bool RotateViewMouseProc::OnMouseFreeMove(HWND hwnd, int point, int flag, const IPoint2& screenPt )
{
	SetCursor(mCursor);
	return mStarted;
}

bool RotateViewMouseProc::OnMouseAbort(HWND hwnd, int point, int flag, const IPoint2& screenPt )
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

int RotateViewMouseProc::proc( 
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
// Rotate View Command Mode
//==============================================================================
RotateViewCommandMode& RotateViewCommandMode::GetInstance()
{
	static RotateViewCommandMode theRotateViewCommandMode;
	return theRotateViewCommandMode;
}

MouseCallBack* RotateViewCommandMode::MouseProc(int *numPoints) 
{ 
	*numPoints = 2; 
	return &proc; 
}

void RotateViewCommandMode::EnterMode() 
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
	ip->PushPrompt(GetString(IDS_ROTATE_MODE));
}

void RotateViewCommandMode::ExitMode() 
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
