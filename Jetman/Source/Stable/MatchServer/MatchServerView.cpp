// MatchServerView.cpp : CMatchServerView Ŭ������ ����
//

#include "stdafx.h"
#include "MatchServer.h"

#include "MatchServerDoc.h"
#include "MatchServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMatchServerView

IMPLEMENT_DYNCREATE(CMatchServerView, CView)

BEGIN_MESSAGE_MAP(CMatchServerView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CMatchServerView ����/�Ҹ�

CMatchServerView::CMatchServerView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CMatchServerView::~CMatchServerView()
{
}

BOOL CMatchServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CMatchServerView �׸���

void CMatchServerView::OnDraw(CDC* /*pDC*/)
{
	CMatchServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CMatchServerView �μ�

BOOL CMatchServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CMatchServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CMatchServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CMatchServerView ����

#ifdef _DEBUG
void CMatchServerView::AssertValid() const
{
	CView::AssertValid();
}

void CMatchServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMatchServerDoc* CMatchServerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMatchServerDoc)));
	return (CMatchServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CMatchServerView �޽��� ó����

void CMatchServerView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	GetDocument()->SetTitle("MatchServerView");
}
