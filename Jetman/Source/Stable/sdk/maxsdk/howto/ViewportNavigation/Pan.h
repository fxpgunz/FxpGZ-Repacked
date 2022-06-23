#pragma once

/**********************************************************************
*<
FILE: Pan.h

DESCRIPTION:	

CREATED BY: 

HISTORY: 

*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/

#include "ViewportNavigation.h"

class PanViewMouseProc : public MouseCallBack 
{
	public:	
		PanViewMouseProc();
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

		Point2 mOldScreenPt;
		ViewExp10* mViewport;
		Matrix3 mOldViewportMatrix;
		HCURSOR mCursor, mOldCursor;
		bool mStarted;

		friend class PanViewCommandMode;
};

// Singleton
class PanViewCommandMode : public CommandMode, public MaxSDK::Util::Noncopyable
{
	public:
		int Class() { return VIEWPORT_COMMAND; }
		int ID() { return CID_PANVIEW; }
		MouseCallBack* MouseProc(int* numPoints);
		ChangeForegroundCallback *ChangeFGProc() { return NULL; }
		BOOL ChangeFG( CommandMode *oldMode ) { return FALSE; }
		void EnterMode();
		void ExitMode();
		static PanViewCommandMode& GetInstance();

	private:
		PanViewCommandMode() : mCursor(NULL), mOldCursor(NULL) { };

	private:
		PanViewMouseProc proc;
		HCURSOR mCursor, mOldCursor;
};
