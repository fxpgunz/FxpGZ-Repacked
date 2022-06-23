#include "stdafx.h"
#include "MZFileSystem.h"
#include "MMatchRuleQuest.h"
#include "MSacrificeQItemTable.h"



bool MSacrificeQItemTable::ReadXml( const char* szFileName, MZFileSystem* pFileSystem)
{
	if (0 == szFileName)
		return false;

	rapidxml::xml_document<> doc;
	MZFile mzf;

	if (pFileSystem)
	{
		if (!mzf.Open(szFileName, pFileSystem))
		{
			if (!mzf.Open(szFileName))
				return false;
		}
	}
	else
	{
		if (!mzf.Open(szFileName))
			return false;
	}

	char* buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());
	if (!doc.parse<0>(buffer)) {
		mlog("error parsing sacrifice table.\n");
		delete[] buffer;
		return false;
	}
	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), MSQITC_ITEM) == 0)
			ParseTable(itor);
	}

	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;
}


void MSacrificeQItemTable::ParseTable( rapidxml::xml_node<>* element )
{
//	char szAttrName[ 256 ];
//	char szAttrValue[ 1024 ];

	int					nQL;
	MSacrificeQItemInfo SacriQItemInfo;

	memset( &SacriQItemInfo, 0, sizeof(MSacrificeQItemInfo) );

	strcpy_s(SacriQItemInfo.m_szMap, element->first_attribute(MSQITC_MAP)->value());
	SacriQItemInfo.m_nQL = atoi(element->first_attribute(MSQITC_QL)->value());
	SacriQItemInfo.m_nDefaultQItemID = atoi(element->first_attribute(MSQITC_DIID)->value());
	SacriQItemInfo.m_nSpecialQItemID1 = atoi(element->first_attribute(MSQITC_SIID1)->value());
	SacriQItemInfo.m_nSpecialQItemID2 = atoi(element->first_attribute(MSQITC_SIID2)->value());
	SacriQItemInfo.m_nScenarioID = atoi(element->first_attribute(MSQITC_SID)->value());

	insert( value_type(nQL, SacriQItemInfo) );
}


int MSacrificeQItemTable::FindSacriQItemInfo( const int nQL, MQuestSacrificeSlot* pSacrificeSlot, int& outResultQL )
{

	if( 0 == nQL )
	{
		outResultQL = nQL;
		if( (0 == pSacrificeSlot[0].GetItemID()) && (0 == pSacrificeSlot[1].GetItemID()) )
			return MSQITRES_NOR;
		else
			return MSQITRES_INV;
	}

	MSacrificeQItemTable::iterator iter, itUpper;

	for( int i = 1; i <= nQL; ++i )
	{
		iter = lower_bound( i );
		if( end() == iter )
			return MSQITRES_ERR;

		itUpper = upper_bound( i );

		outResultQL = i;

		for( ; iter != itUpper; ++iter )
		{
//#ifdef _DEBUG
//			mlog( "CurQL:%d\n", i );
//			mlog( "FindSacriQItemInfo - SacriSlot1:%d\n", pSacrificeSlot[0].GetItemID() );
//			mlog( "FindSacriQItemInfo - SacriSlot2:%d\n", pSacrificeSlot[1].GetItemID() );
//			mlog( "FindSacriQItemInfo - DefItemID:%d\n", iter->second.GetDefQItemID() );
//			mlog( "FindSacriQItemInfo - SpecItemID1:%d\n", iter->second.GetSpeQItemID1() );
//			mlog( "FindSacriQItemInfo - SpecItemID2:%d\n\n", iter->second.GetSpeQItemID2() );
//#endif
			if( ((iter->second.GetDefQItemID() == pSacrificeSlot[0].GetItemID()) && (0 == pSacrificeSlot[1].GetItemID()))  ||
				((iter->second.GetDefQItemID() == pSacrificeSlot[1].GetItemID()) && (0 == pSacrificeSlot[0].GetItemID())) )
			{
				// 일반 시나리오.
				return MSQITRES_NOR;
			}
			else if( ((iter->second.GetSpeQItemID1() == pSacrificeSlot[0].GetItemID()) && (iter->second.GetSpeQItemID2() == pSacrificeSlot[1].GetItemID())) ||
				((iter->second.GetSpeQItemID1() == pSacrificeSlot[1].GetItemID()) && (iter->second.GetSpeQItemID2() == pSacrificeSlot[0].GetItemID())) )
			{
				// 특별 시나리오.
				return MSQITRES_SPC;
			}
			else if( (0 == pSacrificeSlot[0].GetItemID()) && (0 == pSacrificeSlot[1].GetItemID()) )
			{
				// 양쪽이 다 빈 슬롯. 
				// 양쪽이 다 비어있을경우 QL : 1에서 검색이 끝나기 때문에 outResultQL은 1로 설정이 됨.
				return MSQITRES_EMP;
			}
			else if( pSacrificeSlot[0].GetItemID() == pSacrificeSlot[1].GetItemID() )
			{
				// 같은 아이템 중복.
				outResultQL = 1;
				return MSQITRES_DUP;
			}
		}
	}

	// 여기까지 내려오면 테이블에 등록되지 않은 아이템.
	outResultQL = 1;
	return MSQITRES_INV;
}

bool MSacrificeQItemTable::TestInitTable()
{
	return true;
}