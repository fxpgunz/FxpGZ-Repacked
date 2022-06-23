#pragma once


// CCommandLogView ���Դϴ�.

class MCommand;

class CCommandLogView : public CListView
{
	DECLARE_DYNCREATE(CCommandLogView)

protected:
	CCommandLogView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CCommandLogView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	enum CCommandType{
		CCT_LOCAL = 0,
		CCT_SEND = 1,
		CCT_RECEIVE = 2,
	};
	void AddCommand(unsigned long int nGlobalClock, CCommandType t, MCommand* pCmd);
};


