#include "stdafx.h"
#include "MActorDef.h"


MActorDefManager::~MActorDefManager()
{
	Clear();
}

bool MActorDefManager::ReadXml(MZFileSystem* pFileSystem, const char* szFileName)
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

	char* buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());
	if (!doc.parse<0>(buffer)) {
		mlog("Error parsing %s\n", szFileName);
		delete[] buffer;
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	bool bResult = ParseXmlFromStr(buffer,rootNode);

	delete[] buffer;
	doc.clear();
	mzf.Close();

	return bResult;
}

bool MActorDefManager::ParseXmlFromStr(const char* szXmlContent, rapidxml::xml_node<>* rootNode)
{
	Clear();

	if (!szXmlContent) return NULL;

	bool bResult = true;
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{

		if (itor->name()[0] == '#')
			continue;
		if (_stricmp(itor->name(), "ACTOR") == 0)
		{
			MActorDef* pActorDef = new MActorDef;

			pActorDef->SetName(itor->first_attribute("name")->value());
			pActorDef->SetFsmName(itor->first_attribute("ai.fsm")->value());
			pActorDef->SetModelName(itor->first_attribute("model")->value());
			pActorDef->SetMaxHp(atoi(itor->first_attribute("max_hp")->value()));
			pActorDef->SetMaxAp(atoi(itor->first_attribute("max_ap")->value()));
			pActorDef->SetCollisionRadius(strtof(itor->first_attribute("collision.radius")->value(),nullptr));
			pActorDef->SetCollisionHeight(strtof(itor->first_attribute("collision.height")->value(),nullptr));
			pActorDef->SetSpeed(strtof(itor->first_attribute("speed")->value(),nullptr));
			pActorDef->SetRotSpeed(strtof(itor->first_attribute("rotspeed")->value(),nullptr));
			pActorDef->SetGroggyRecoverySpeed(strtof(itor->first_attribute("groggyRecoverPerSec")->value(),nullptr));

			//Custom: missing nodes
			if (_stricmp(itor->first_attribute("neverblasted")->value(), "true") == 0)
				pActorDef->SetNeverBlasted(true);
			else
				pActorDef->SetNeverBlasted(false);

			if (itor->first_attribute("neverblasted.sound") != nullptr)
			{
				pActorDef->SetNeverBlastedSound(itor->first_attribute("neverblasted.sound")->value());
			}
			else
			{
				pActorDef->SetNeverBlastedSound("");
			}

			if (itor->first_attribute("meshpicking") != nullptr) {
				if (_stricmp(itor->first_attribute("meshpicking")->value(), "true") == 0)
					pActorDef->SetMeshPicking(true);
				else
					pActorDef->SetMeshPicking(false);
			}
			else
				pActorDef->SetMeshPicking(false);

			if (itor->first_attribute("grenadecollision") != nullptr) {
				if (_stricmp(itor->first_attribute("grenadecollision")->value(), "true") == 0)
					pActorDef->SetGrenadeCollision(true);
				else
					pActorDef->SetGrenadeCollision(false);
			}
			else
				pActorDef->SetGrenadeCollision(false);

			if (itor->first_attribute("sound.die") != nullptr) {
				pActorDef->SetSoundDie(itor->first_attribute("sound.die")->value());
			}
			else
				pActorDef->SetSoundDie("");

			if (itor->first_attribute("forced.collup120") != nullptr) {
				if (_stricmp(itor->first_attribute("forced.collup120")->value(), "true") == 0)
					pActorDef->SetForcedCollup120(true);
				else
					pActorDef->SetForcedCollup120(false);
			}
			else
				pActorDef->SetForcedCollup120(false);

			if (itor->first_attribute("boss") != nullptr) {
				if (_stricmp(itor->first_attribute("boss")->value(), "true") == 0)
					pActorDef->SetBoss(true);
				else
					pActorDef->SetBoss(false);
			}
			else
				pActorDef->SetBoss(false);

			if (itor->first_attribute("noshadow") != nullptr) {
				if (_stricmp(itor->first_attribute("noshadow")->value(), "true") == 0)
					pActorDef->SetDrawShadow(true);
				else
					pActorDef->SetDrawShadow(false);
			}
			else
				pActorDef->SetDrawShadow(false);

			if (itor->first_attribute("type") != nullptr) {
				pActorDef->SetActorType(itor->first_attribute("type")->value());
			}
			else
				pActorDef->SetActorType("");

			if (itor->first_attribute("scale") != nullptr) {
				pActorDef->SetScale(strtof(itor->first_attribute("scale")->value(),nullptr));
			}
			else
				pActorDef->SetScale(0);


			// 에러 체크
			if (0 == strcmp(pActorDef->GetName(), "")) {
				mlog("ERROR: xml actor node has no name attr.\n"); _ASSERT(0); bResult = false;
			}
			if (0 == strcmp(pActorDef->GetFsmName(), "")) {
				mlog("ERROR: xml actor node has no fsm name attr.\n"); _ASSERT(0); bResult = false;
			}
			if (0 == strcmp(pActorDef->GetModelName(), "")) {
				mlog("ERROR: xml actor node has no name attr.\n"); _ASSERT(0); bResult = false;
			}
			if (0 == pActorDef->GetMaxHp()) {
				mlog("ERROR: xml actor node has no max_hp attr.\n"); _ASSERT(0); bResult = false;
			}
			if (0 == pActorDef->GetCollisionRadius()) {
				mlog("ERROR: xml actor node has no collision.radius attr.\n"); _ASSERT(0); bResult = false;
			}
			if (0 == pActorDef->GetCollisionHeight()) {
				mlog("ERROR: xml actor node has no collision.height attr.\n"); _ASSERT(0); bResult = false;
			}
			if (0 == pActorDef->GetSpeed()) {
				mlog("ERROR: xml actor node has no speed attr.\n"); _ASSERT(0); bResult = false;
			}
			if (0 == pActorDef->GetRotSpeed()) {
				mlog("ERROR: xml actor node has no rotspeed attr.\n"); _ASSERT(0); bResult = false;
			}
			//Custom: missing nodes


			if (!bResult)
			{
				delete pActorDef;
				break;
			}

			// 추가
			if (!AddActorDef(pActorDef))
			{
				mlog("ERROR: xml actor node name duplicated.");
				delete pActorDef;
				bResult = false;
				break;
			}
		}

	}
	// 중간에 파싱이 실패하면 읽었던 내용을 다 버린다
	if (!bResult)
		Clear();

	return bResult;
}

MActorDef* MActorDefManager::GetDef(const char* szActorName)
{
	ItorActorDef it = m_mapActorDef.find(szActorName);
	if (it != m_mapActorDef.end())
		return it->second;
	return NULL;
}

bool MActorDefManager::AddActorDef( MActorDef* pDef )
{
	// 이미 존재하는 이름이면 실패
	if (GetDef(pDef->GetName()))
		return false;

	m_mapActorDef[pDef->GetName()] = pDef;
	return true;
}

void MActorDefManager::Clear()
{
	for (ItorActorDef it=m_mapActorDef.begin(); it!=m_mapActorDef.end(); ++it)
		delete it->second;
	m_mapActorDef.clear();
}