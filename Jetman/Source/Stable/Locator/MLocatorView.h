#pragma once


// MLocatorView ���Դϴ�.

class MLocatorView : public CListView
{
	DECLARE_DYNCREATE(MLocatorView)

protected:
	MLocatorView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~MLocatorView();

public :
	void UpdateView();

private :
	bool InitColumns();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
};

MLocatorView* GetLocatorView();