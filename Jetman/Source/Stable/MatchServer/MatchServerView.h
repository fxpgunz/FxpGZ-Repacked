// MatchServerView.h : iCMatchServerView Ŭ������ �������̽�
//


#pragma once


class CMatchServerDoc;


class CMatchServerView : public CView
{
protected: // serialization������ ��������ϴ�.
	CMatchServerView();
	DECLARE_DYNCREATE(CMatchServerView)

// Ư��
public:
	CMatchServerDoc* GetDocument() const;

// �۾�
public:

// ������
	public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ����
public:
	virtual ~CMatchServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

#ifndef _DEBUG  // MatchServerView.cpp�� ����� ����
inline CMatchServerDoc* CMatchServerView::GetDocument() const
   { return reinterpret_cast<CMatchServerDoc*>(m_pDocument); }
#endif

