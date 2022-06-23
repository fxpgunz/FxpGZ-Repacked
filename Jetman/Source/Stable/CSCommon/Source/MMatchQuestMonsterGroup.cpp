/////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MMatchQuestMonsterGroup.h"
#include "MZFileSystem.h"
#include "FileInfo.h"

#define MTOK_MONSTERGROUP	"GROUP"
#define MTOK_NPC			"NPC"
#define MTOK_ATTR_ID		"id"
#define MTOK_ATTR_NAME		"name"


MNPCGroupMgr::MNPCGroupMgr()
{

}

MNPCGroupMgr::~MNPCGroupMgr()
{
	Clear();
}

MNPCGroupMgr* MNPCGroupMgr::GetInstance()
{
static MNPCGroupMgr g_NPCGroupMgr;
return &g_NPCGroupMgr;
}

void MNPCGroupMgr::Clear()
{
	while (!empty()) {
		MNPCGroup* pGroup = (*begin()).second;
		delete pGroup;
		pGroup = NULL;
		erase(begin());
	}
}


MNPCGroup* MNPCGroupMgr::GetGroup(const string& strName)
{
	iterator itor = find(strName);
	if (itor != end()) {
		return (*itor).second;
	}
	return NULL;
}

MNPCGroup* MNPCGroupMgr::GetGroup(int nGroupID)
{
	iterator it;
	MNPCGroup* pGroup = NULL;

	for (it = begin(); it != end(); ++it) {

		pGroup = (*it).second;

		if (pGroup) {
			if (pGroup->GetID() == nGroupID) {
				return pGroup;
			}
		}
	}
	return NULL;
}

bool MNPCGroupMgr::ReadXml(const char* szFileName, MZFileSystem* pFileSystem)
{
	rapidxml::xml_document<> doc;
	MZFile mzf;

	if (pFileSystem) {
		if (!mzf.Open(szFileName, pFileSystem)) {
			if (!mzf.Open(szFileName))
				return false;
		}
	}
	else {

		if (!mzf.Open(szFileName))

			return false;
	}

	char* buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer, mzf.GetLength());
	if (!doc.parse<0>(buffer)) {
		mlog("error parsing monstergroup.xml\n");
		delete[] buffer;
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), MTOK_MONSTERGROUP) == 0)
			ParseRule(itor);
	}

	delete[] buffer;
	doc.clear();
	mzf.Close();
	return true;
}

void MNPCGroupMgr::ParseRule(rapidxml::xml_node<>* pElement)
{
	MNPCGroup* pGroup = new MNPCGroup;
	pGroup->SetID(atoi(pElement->first_attribute(MTOK_ATTR_ID)->value()));
	pGroup->SetName(pElement->first_attribute(MTOK_ATTR_NAME)->value());

	rapidxml::xml_node<>* childNode = pElement->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), MTOK_NPC) == 0)
			pGroup->AddNpc(itor->first_attribute(MTOK_ATTR_NAME)->value());
	}

	insert(value_type(pGroup->GetName(), pGroup));
}

