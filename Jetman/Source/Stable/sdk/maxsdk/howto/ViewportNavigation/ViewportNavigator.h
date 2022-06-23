/**********************************************************************
 *<
	FILE: ViewportNavigator.h

	DESCRIPTION:	

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2007, All Rights Reserved.
 **********************************************************************/
#ifndef __VIEWPORT_NAVIGATOR__H
#define __VIEWPORT_NAVIGATOR__H

#include "ViewportNavigation.h"

// Singleton
class ViewportNavigator : public UtilityObj, public MaxSDK::Util::Noncopyable
{
	public:
		virtual ~ViewportNavigator();	

		void BeginEditParams(Interface *ip, IUtil *iu);
		void EndEditParams(Interface *ip, IUtil *iu);

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);
		
		void DeleteThis();

		static ViewportNavigator& GetInstance();
		static Class_ID GetClassID();

	private:
		class UIUpdater : public CommandModeChangedCallback
		{
			public: 
				UIUpdater() {}
				virtual void ModeChanged(CommandMode *oldM, CommandMode *newM); 
		};
		UIUpdater mUIUpdater;
		friend class UIUpdater;

		static void UpdateUI(HWND hDlgWnd, UINT btnID, bool btnActive);
		HWND GetDlgHWnd() { return mDlgWnd; }

	private:
		HWND mDlgWnd;
		IUtil* mpIUtil;
		Interface* mpInterface;
		static const Class_ID mClassID;	

		// UI
		static INT_PTR CALLBACK DlgProc(
			HWND hWnd, 
			UINT msg, 
			WPARAM wParam, 
			LPARAM lParam);

		// Singleton
		ViewportNavigator();
};

#endif