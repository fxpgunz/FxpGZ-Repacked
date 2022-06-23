// LocatorDoc.h : CLocatorDoc Ŭ������ �������̽�
//


#pragma once

class MLocator;

class CLocatorDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CLocatorDoc();
	DECLARE_DYNCREATE(CLocatorDoc)

// Ư��
public:

// �۾�
public:
	void Run();

	const MLocator* GetLocator() const { return m_pLocator; }

private :
	bool CreateLocator();
	void ReleaseLocator();

private :
	MLocator*	m_pLocator;

// ������
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ����
public:
	virtual ~CLocatorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOutputlocatorstatusinfo();
};


