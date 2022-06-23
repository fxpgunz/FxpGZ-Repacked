// LocatorView.cpp : CLocatorView Ŭ������ ����
//

#include "stdafx.h"
#include "Locator.h"

#include "LocatorDoc.h"
#include "LocatorView.h"
#include ".\locatorview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLocatorView

IMPLEMENT_DYNCREATE(CLocatorView, CView)

BEGIN_MESSAGE_MAP(CLocatorView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CLocatorView ����/�Ҹ�

CLocatorView::CLocatorView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CLocatorView::~CLocatorView()
{
}

BOOL CLocatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CLocatorView �׸���

void CLocatorView::OnDraw(CDC* /*pDC*/)
{
	CLocatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CLocatorView �μ�

BOOL CLocatorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CLocatorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CLocatorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CLocatorView ����

#ifdef _DEBUG
void CLocatorView::AssertValid() const
{
	CView::AssertValid();
}

void CLocatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLocatorDoc* CLocatorView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLocatorDoc)));
	return (CLocatorDoc*)m_pDocument;
}
#endif //_DEBUG


// CLocatorView �޽��� ó����