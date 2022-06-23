// CommandLogView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CommandLogView.h"
#include "MCommand.h"
#include "MMatchGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCommandLogView

IMPLEMENT_DYNCREATE(CCommandLogView, CListView)

CCommandLogView::CCommandLogView()
{
}

CCommandLogView::~CCommandLogView()
{
}

BEGIN_MESSAGE_MAP(CCommandLogView, CListView)
END_MESSAGE_MAP()


// CCommandLogView �����Դϴ�.

#ifdef _DEBUG
void CCommandLogView::AssertValid() const
{
	CListView::AssertValid();
}

void CCommandLogView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// CCommandLogView �޽��� ó�����Դϴ�.

void CCommandLogView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	CListView::OnInitialUpdate();

	// TODO: GetListCtrl()�� ȣ���Ͽ� �ش� list ��Ʈ���� ���� �׼��������ν�
	// ListView�� �׸����� ä�� �� �ֽ��ϴ�.

	// this code only works for a report-mode list view
	ASSERT(GetStyle() & LVS_REPORT);

	// Gain a reference to the list control itself
	CListCtrl& theCtrl = GetListCtrl();

	// Insert a column. This override is the most convenient.
	theCtrl.InsertColumn(0, _T("No"), LVCFMT_LEFT);
	theCtrl.InsertColumn(1, _T("Time"), LVCFMT_LEFT);
	theCtrl.InsertColumn(2, _T("Type"), LVCFMT_LEFT);
	theCtrl.InsertColumn(3, _T("Sender"), LVCFMT_LEFT);
	theCtrl.InsertColumn(4, _T("Receiver"), LVCFMT_LEFT);
	theCtrl.InsertColumn(5, _T("Command"), LVCFMT_LEFT);

	#ifdef LEAK_TEST
		theCtrl.InsertColumn(6, _T("Memory"), LVCFMT_LEFT);
	#endif

	// Set reasonable widths for our columns
	theCtrl.SetColumnWidth(0, 50);
	theCtrl.SetColumnWidth(1, 80);
	theCtrl.SetColumnWidth(2, 50);
	theCtrl.SetColumnWidth(3, 80);
	theCtrl.SetColumnWidth(4, 80);
	theCtrl.SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
	#ifdef LEAK_TEST
		theCtrl.SetColumnWidth(6, 80);
	#endif

}

BOOL CCommandLogView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_REPORT;

	return CListView::PreCreateWindow(cs);
}


void CCommandLogView::AddCommand(unsigned long int nGlobalClock, CCommandType t, MCommand* pCmd)
{
	static unsigned long int m_stLogCount = 0;
	m_stLogCount++;

	char temp[4096]="";
	char szParam[1024]="";
	//sprintf(temp, "%d: %s", nGlobalClock, pCmd->m_pCommandDesc->GetName());
	sprintf(temp, "%s", pCmd->m_pCommandDesc->GetName());
	for(int i=0; i<pCmd->GetParameterCount(); i++){
		pCmd->GetParameter(i)->GetString(szParam);
		sprintf(temp, "%s %s(%s)", temp, pCmd->GetParameter(i)->GetClassName(), szParam);
		
	}

	CListCtrl& theCtrl = GetListCtrl();

	char szNO[32]="";
	itoa(m_stLogCount, szNO, 10);
	int nItemCount = theCtrl.GetItemCount();

	theCtrl.InsertItem(LVIF_TEXT|LVIF_STATE, nItemCount, szNO, 0, LVIS_SELECTED, 0, 0);

	char szGlobalClock[128]="";
	sprintf(szGlobalClock, "%d", nGlobalClock);
	theCtrl.SetItemText(nItemCount, 1, szGlobalClock);
	char szType[64] = "Unknown";
	if(t==CCT_LOCAL) strcpy(szType, "Local");
	else if(t==CCT_SEND) strcpy(szType, "Send");
	else if(t==CCT_RECEIVE) strcpy(szType, "Receive");
	char szSenderUID[128];
	sprintf(szSenderUID, "%u:%u", pCmd->m_Sender.High, pCmd->m_Sender.Low);
	char szReceiverUID[128];
	sprintf(szReceiverUID, "%u:%u", pCmd->m_Receiver.High, pCmd->m_Receiver.Low);
	theCtrl.SetItemText(nItemCount, 2, szType);
	theCtrl.SetItemText(nItemCount, 3, szSenderUID);
	theCtrl.SetItemText(nItemCount, 4, szReceiverUID);
	theCtrl.SetItemText(nItemCount, 5, temp);

#ifdef LEAK_TEST
	sprintf(temp, "%6.2f", (float)MGetCurrProcessMemory() / 1024.0f);
	theCtrl.SetItemText(nItemCount, 6, temp);
#endif

	//theCtrl.SetScrollPos(SB_VERT, );
	int nCount = theCtrl.GetItemCount();
	if (nCount > 0)	
		theCtrl.EnsureVisible(nCount-1, TRUE);

#define MAX_LOG_COUNT	1000
	if (nCount > MAX_LOG_COUNT)
		theCtrl.DeleteItem(0);
}
