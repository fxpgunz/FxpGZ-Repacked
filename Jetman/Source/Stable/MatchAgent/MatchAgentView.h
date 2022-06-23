// MatchAgentView.h : iCMatchAgentView Ŭ������ �������̽�
//


#pragma once


class CMatchAgentView : public CView
{
protected: // serialization������ ��������ϴ�.
	CMatchAgentView();
	DECLARE_DYNCREATE(CMatchAgentView)

// Ư��
public:
	CMatchAgentDoc* GetDocument() const;

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
	virtual ~CMatchAgentView();
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

#ifndef _DEBUG  // MatchAgentView.cpp�� ����� ����
inline CMatchAgentDoc* CMatchAgentView::GetDocument() const
   { return reinterpret_cast<CMatchAgentDoc*>(m_pDocument); }
#endif

