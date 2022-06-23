#pragma once

/**********************************************************************
*<
FILE: zoom.h

DESCRIPTION:	

CREATED BY: 

HISTORY: 

*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/

#include "ViewportNavigation.h"

class ZoomViewMouseProc : public MouseCallBack 
{
	public:	
		ZoomViewMouseProc();
		int proc( HWND hwnd, int msg, int point, int flags, IPoint2 m );
	
	private:
		bool NormalStart(HWND hwnd, int point, int flag, const IPoint2& screenPt);
		void Idle();
		bool OnMousePoint(HWND hwnd, int point, int flag, const IPoint2& screenPt);
		bool OnMouseMove(HWND hwnd, int point, int flag, const IPoint2& screenPt);
		bool OnMouseFreeMove(HWND hwnd, int point, int flag, const IPoint2& screenPt);
		bool OnMouseAbort(HWND hwnd, int point, int flag, const IPoint2& screenPt);
		bool OnKeyboard(HWND hwnd, int point, int flag, const IPoint2& screenPt);
		void SetCursor(HCURSOR hCursor);

		IPoint2 mOldScreenPt;
		ViewExp10* mViewport;
		Matrix3 mOldViewportMatrix;
		HCURSOR mCursor, mOldCursor;
		bool mStarted;

		friend class ZoomViewCommandMode;
};

// Singleton
class ZoomViewCommandMode : public CommandMode, public MaxSDK::Util::Noncopyable
{
	public:
		int Class() { return VIEWPORT_COMMAND; }
		int ID() { return CID_ZOOMVIEW; }
		MouseCallBack* MouseProc(int* numPoints);
		ChangeForegroundCallback *ChangeFGProc() { return NULL; }
		BOOL ChangeFG( CommandMode *oldMode ) { return FALSE; }
		void EnterMode();
		void ExitMode();
		static ZoomViewCommandMode& GetInstance();

	private:
		ZoomViewCommandMode() : mCursor(NULL), mOldCursor(NULL) { };

	private:
		ZoomViewMouseProc proc;
		HCURSOR mCursor, mOldCursor;
};
