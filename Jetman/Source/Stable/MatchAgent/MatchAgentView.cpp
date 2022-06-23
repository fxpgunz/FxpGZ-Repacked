// MatchAgentView.cpp : CMatchAgentView Ŭ������ ����
//

#include "stdafx.h"
#include "MatchAgent.h"

#include "MatchAgentDoc.h"
#include "MatchAgentView.h"
#include ".\matchagentview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMatchAgentView

IMPLEMENT_DYNCREATE(CMatchAgentView, CView)

BEGIN_MESSAGE_MAP(CMatchAgentView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CMatchAgentView ����/�Ҹ�

CMatchAgentView::CMatchAgentView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CMatchAgentView::~CMatchAgentView()
{
}

BOOL CMatchAgentView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CMatchAgentView �׸���

void CMatchAgentView::OnDraw(CDC* /*pDC*/)
{
	CMatchAgentDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CMatchAgentView �μ�

BOOL CMatchAgentView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CMatchAgentView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CMatchAgentView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CMatchAgentView ����

#ifdef _DEBUG
void CMatchAgentView::AssertValid() const
{
	CView::AssertValid();
}

void CMatchAgentView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMatchAgentDoc* CMatchAgentView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMatchAgentDoc)));
	return (CMatchAgentDoc*)m_pDocument;
}
#endif //_DEBUG


// CMatchAgentView �޽��� ó����

void CMatchAgentView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	GetDocument()->SetTitle("MatchAgentView");
}
