// LocatorView.h : iCLocatorView Ŭ������ �������̽�
//


#pragma once


class CLocatorView : public CView
{
protected: // serialization������ ��������ϴ�.
	CLocatorView();
	DECLARE_DYNCREATE(CLocatorView)

// Ư��
public:
	CLocatorDoc* GetDocument() const;

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
	virtual ~CLocatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
};

#ifndef _DEBUG  // LocatorView.cpp�� ����� ����
inline CLocatorDoc* CLocatorView::GetDocument() const
   { return reinterpret_cast<CLocatorDoc*>(m_pDocument); }
#endif

