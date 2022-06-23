// MLocatorView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Locator.h"
#include "MLocatorView.h"
#include ".\mlocatorview.h"
#include "LocatorDoc.h"
#include "MLocator.h"
#include "MServerStatus.h"

MLocatorView* g_pLocatorView = 0;

MLocatorView* GetLocatorView()
{
	return g_pLocatorView;
}


// MLocatorView

IMPLEMENT_DYNCREATE(MLocatorView, CListView)

MLocatorView::MLocatorView()
{
	g_pLocatorView = this;
}

MLocatorView::~MLocatorView()
{
}

BEGIN_MESSAGE_MAP(MLocatorView, CListView)
END_MESSAGE_MAP()


// MLocatorView �����Դϴ�.

#ifdef _DEBUG
void MLocatorView::AssertValid() const
{
	CListView::AssertValid();
}

void MLocatorView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// MLocatorView �޽��� ó�����Դϴ�.

BOOL MLocatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	cs.style |= LVS_SHOWSELALWAYS | LVS_REPORT;

	return CListView::PreCreateWindow(cs);
}


BOOL MLocatorView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	BOOL bCreated = CListView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	if( bCreated )
	{
		CListCtrl& lc = GetListCtrl();
		lc.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

		if( !InitColumns() ) return FALSE;
		
		return TRUE;
	}

	return FALSE;
}


bool MLocatorView::InitColumns()
{
	CListCtrl& lc = GetListCtrl();

	lc.InsertColumn( 0, "Max Server count", LVCFMT_LEFT, 150 );
	lc.InsertColumn( 1, "Current Server count", LVCFMT_LEFT, 150 );
	lc.InsertColumn( 2, "Stop Server ID", LVCFMT_LEFT, 200 );

	return TRUE;
}


void MLocatorView::UpdateView()
{
	POSITION p = theApp.GetFirstDocTemplatePosition(); 
	CDocTemplate* pTemplate = theApp.GetNextDocTemplate(p); 
	p = pTemplate->GetFirstDocPosition(); 
	CLocatorDoc* pDoc = (CLocatorDoc*)pTemplate->GetNextDoc(p); 
	if( 0 != pDoc )
	{
		const MServerStatusMgr* pServerStatusMgr = pDoc->GetLocator()->GetServerStatusMgr();
		if( (0 != pServerStatusMgr) && (0 < pServerStatusMgr->GetSize()) )
		{
			CListCtrl& lc = GetListCtrl();

			char szBuf[ 128 ];
			string strDeadServerIDList;
			const int nDeadServerCount = pServerStatusMgr->GetDeadServerCount();

			_snprintf( szBuf, 127, "%d", pServerStatusMgr->GetSize() );
			lc.InsertItem( 0, szBuf );

			_snprintf( szBuf, 127, "Run:%d, Dead:%d", pServerStatusMgr->GetLiveServerCount(), nDeadServerCount );
			lc.SetItemText( 0, 1, szBuf );
			
			for( int i = 0; i < nDeadServerCount; ++i )
			{
				_snprintf( szBuf, 127, "%d,  ", pServerStatusMgr->GetDeadServerIDList()[i] );
				strDeadServerIDList += szBuf;
			}
			strDeadServerIDList += "\0";

			lc.SetItemText( 0, 2, strDeadServerIDList.c_str() );
		}
	}
}