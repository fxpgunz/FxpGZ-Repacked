#include "stdafx.h"
#include "MCompanionDef.h"

MCompanionDefMgr::~MCompanionDefMgr()
{
	Clear();
}

void MCompanionDefMgr::Clear()
{
	for (auto itor = m_actorDefMgr.begin(); itor != m_actorDefMgr.end(); ++itor)
		delete itor->second;

	m_actorDefMgr.clear();
}

bool MCompanionDefMgr::ReadXml(const char* fileName, MZFileSystem* pFileSystem)
{
	rapidxml::xml_document<> doc;

	MZFile mzf;
	if (pFileSystem) {
		if (!mzf.Open(fileName, pFileSystem)) {
			if (!mzf.Open(fileName))
				return false;
		}
	}
	else {

		if (!mzf.Open(fileName))
			return false;
	}

	char* buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer, mzf.GetLength());
	if (!doc.parse<0>(buffer)) {
		mlog("Error parsing %s\n", fileName);
		delete[] buffer;
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();

	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), "COMPANION") == 0)
		{
			MCompanionDef* companionDef = new MCompanionDef;

			companionDef->SetCompanionName(itor->first_attribute("name")->value());
			companionDef->SetCompanionModel(itor->first_attribute("model")->value());
			companionDef->SetMaxHP(atoi(itor->first_attribute("max_hp")->value()));
			companionDef->SetMaxAP(atoi(itor->first_attribute("max_ap")->value()));
			companionDef->SetCollRadius(atoi(itor->first_attribute("colradius")->value()));
			companionDef->SetCollHeight(atoi(itor->first_attribute("colheight")->value()));
			companionDef->SetDrawShadow(atob(itor->first_attribute("shadow")->value()));
			companionDef->SetMeshPicking(atob(itor->first_attribute("meshpicking")->value()));

			if (!AddCompanionDef(companionDef))
			{
				mlog("ERROR: companion actor node name duplicated.");
				delete companionDef;
				return false;
			}
		}
	}


	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;
}

bool MCompanionDefMgr::AddCompanionDef(MCompanionDef* companionDef)
{
	if (GetCompanion(companionDef->GetCompanionName().c_str()))
		return false;

	m_actorDefMgr[companionDef->GetCompanionName()] = companionDef;

	return true;
}

MCompanionDef* MCompanionDefMgr::GetCompanion(const char* companionName)
{
	auto itor = m_actorDefMgr.find(companionName);
	if (itor != m_actorDefMgr.end())
		return itor->second;

	return nullptr;
}