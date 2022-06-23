#include "stdafx.h"
#include "ZTips.h"
#include "rapidxml.hpp"
#include "Physics.h"
#include "ZFilePath.h"
#include "ZConfiguration.h"


#include "stdafx.h"
#include "ZTips.h"
//#include "Physics.h"
//#include "ZFilePath.h"
#include "ZConfiguration.h"

#define ZTOK_TIPS			"TIPS"
#define ZTOK_TIPS_MSG		"MSG"

#define ZTOK_TIPS_ATTR_CATEGORY		"category"
#define ZTOK_TIPS_MSG_ATTR_ID		"id"


ZTips::ZTips() : m_bIsShowedNewbieTips(false)
{

}


bool ZTips::Initialize(MZFileSystem* pfs, const MLanguage LangID)
{
	MZFile mzf;

	char szTipsPath[64];
	sprintf(szTipsPath, "%s%s", ZGetConfiguration()->GetLanguagePath(), FILENAME_TIPS);

	if (!mzf.Open(szTipsPath, pfs))
		return false;

	char *buffer = new char[mzf.GetLength() + 1];
	mzf.Read(buffer, mzf.GetLength());
	buffer[mzf.GetLength()] = 0;


	mlog("Load XML from memory : %s(0x%04X)", szTipsPath, LangID);

	rapidxml::xml_document<> doc;
	doc.parse<0>(buffer);

	rapidxml::xml_node<>* rootNode = doc.first_node();
	if (rootNode == nullptr) {
		mlog("- FAILED");
		return false;
	}

	for (auto it = rootNode->first_node(); it; it = it->next_sibling())
	{
		if (_stricmp(it->name(), "TIPS") == 0)
		{
			ParseTips(it);
		}
	}
	delete[] buffer;
	mzf.Close();
	doc.clear();

	return true;
}

void ZTips::ParseTips(rapidxml::xml_node<>* pElement)
{
	int nCategory = 0;
	nCategory = atoi(pElement->first_attribute()->value());

	for (auto it = pElement->first_node(); it; it = it->next_sibling())
	{
		if (_stricmp(it->name(), "MSG") == 0)
		{
			int nMsgID = -1;
			if (it->first_attribute("id") != nullptr)
			{
				nMsgID = atoi(it->first_attribute("id")->value());
			}
			// data 입력
			m_StringVector.push_back(string(it->first_node()->value()));

			ZTIPS_MSG msg;
			msg.nMSGID = nMsgID;
			msg.nVectorIndex = (int)m_StringVector.size() - 1;

			m_MsgsVectors[nCategory].push_back(msg);
		}
	}
}

void ZTips::Finalize()
{
	m_StringVector.clear();
}

const char* ZTips::GetRandomTips()
{
	int nSize = (int)m_StringVector.size();
	if (nSize == 0) return "";


	int nIndex = rand() % nSize;
	if ((nIndex < 0) || (nIndex >= nSize)) return "";
	return m_StringVector[nIndex].c_str();
}

const char* ZTips::GetTips(ZTIPS_CATEGORY nCategory, int nID)
{
	vector<int>		nIndices;
	for (int i = 0; i < (int)m_MsgsVectors[(int)nCategory].size(); i++)
	{
		if (nID == m_MsgsVectors[(int)nCategory][i].nMSGID)
		{
			nIndices.push_back(m_MsgsVectors[(int)nCategory][i].nVectorIndex);
		}
	}

	int nStrIndex = 0;
	if (nIndices.empty())
	{
		_ASSERT(0);	// 보여줄 메세지가 없다.
		return "";
	}
	else
	{
		int nRandIndex = rand() % (int)nIndices.size();
		nStrIndex = nIndices[nRandIndex];
	}

	return m_StringVector[nStrIndex].c_str();
}


