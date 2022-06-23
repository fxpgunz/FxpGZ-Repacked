// LocatorDoc.cpp : CLocatorDoc Ŭ������ ����
//

#include "stdafx.h"
#include "Locator.h"

#include "LocatorDoc.h"
#include "MLocator.h"
#include ".\locatordoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLocatorDoc

IMPLEMENT_DYNCREATE(CLocatorDoc, CDocument)

BEGIN_MESSAGE_MAP(CLocatorDoc, CDocument)
	ON_COMMAND(ID_OutputLocatorStatusInfo, OnOutputlocatorstatusinfo)
END_MESSAGE_MAP()


// CLocatorDoc ����/�Ҹ�

CLocatorDoc::CLocatorDoc() : m_pLocator( 0 ) 
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CLocatorDoc::~CLocatorDoc()
{
	ReleaseLocator();
}

BOOL CLocatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ �ٽ� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	if( !CreateLocator() )
		return FALSE;

	return TRUE;
}


bool CLocatorDoc::CreateLocator()
{
	ASSERT( 0 == m_pLocator );

	if( 0 == m_pLocator )
	{
		m_pLocator = new MLocator;
		if( 0 == m_pLocator ) 
			return false;
	}
	else
		m_pLocator->Destroy();

	return m_pLocator->Create();
}


void CLocatorDoc::ReleaseLocator()
{
	m_pLocator->Destroy();
	delete m_pLocator;
	m_pLocator = 0;
}


void CLocatorDoc::Run()
{
	if( 0 != m_pLocator )
	{
		m_pLocator->Run();
	}
}


// CLocatorDoc serialization

void CLocatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CLocatorDoc ����

#ifdef _DEBUG
void CLocatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLocatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLocatorDoc ���

void CLocatorDoc::OnOutputlocatorstatusinfo()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	if( 0 != m_pLocator )
		m_pLocator->DumpLocatorStatusInfo();
}
