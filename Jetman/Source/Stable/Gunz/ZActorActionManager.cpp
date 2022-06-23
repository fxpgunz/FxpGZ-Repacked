#include "stdafx.h"
#include "ZActorActionManager.h"
#include "ZActorAction.h"


ZActorActionManager::~ZActorActionManager()
{
	Clear();
}

bool ZActorActionManager::ReadXml(MZFileSystem* pFileSystem, const char* szFileName)
{
	char *buffer;
	MZFile mzf;

	if (pFileSystem) {
		if (!mzf.Open(szFileName, pFileSystem)) {
			if (!mzf.Open(szFileName)) {
				return false;
			}
		}
	}
	else {

		if (!mzf.Open(szFileName)) {
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer,mzf.GetLength());

	bool bResult = ParseXmlFromStr(buffer);

	delete[] buffer;
	mzf.Close();

	return bResult;
}

bool ZActorActionManager::ParseXmlFromStr(const char* szXmlContent)
{
	Clear();

	if (!szXmlContent) return NULL;

	rapidxml::xml_document<> doc;
	doc.parse<0>(const_cast<char*>(szXmlContent));
	
	rapidxml::xml_node<>* rootNode = doc.first_node();

	bool bResult = true;
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), "ACTION") == 0)
		{
			ZActorAction* pAction = new ZActorAction;
			if (!ParseXml_Action(pAction, itor))
			{
				mlog("Error parsing zactoraction action node\n");
				bResult = false;
				delete pAction;
				break;
			}

			if (!AddAction(pAction))
			{
				mlog("ERROR: xml action node name duplicated.\n");
				delete pAction;
				bResult = false;
				break;
			}
		}
	}

	// 중간에 파싱이 실패하면 읽었던 내용을 다 버린다
	if (!bResult)
		Clear();

	doc.clear();

	return bResult;
}

bool ZActorActionManager::ParseXml_Action(ZActorAction* pOutAction, rapidxml::xml_node<>* elem)
{
	if (elem->first_attribute("name") == nullptr)
	{
		mlog("Error, node doesn't exist\n");
	}
	pOutAction->SetName(elem->first_attribute("name")->value());
	pOutAction->SetAnimationName(elem->first_attribute("animation")->value());
	if (elem->first_attribute("movinganimation") != nullptr)
	{
		if (_stricmp(elem->first_attribute("movinganimation")->value(), "true") == 0)
			pOutAction->SetMovingAnimation(true);
		else
			pOutAction->SetMovingAnimation(false);
	}
	else
		pOutAction->SetMovingAnimation(false);

	for (auto itor = elem->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), "MELEESHOT") == 0)
		{
			ZActorActionMeleeShot* pMeleeShot = new ZActorActionMeleeShot;
			if (!ParseXml_ActionMeleeShot(pMeleeShot, itor)) {
				delete pMeleeShot;
				return false;
			}
			pOutAction->AddMeleeShot(pMeleeShot);
		}

		else if (_stricmp(itor->name(), "RANGESHOT") == 0)
		{
			ZActorActionRangeShot* pRangeShot = new ZActorActionRangeShot;
			if (!ParseXml_ActionRangeShot(pRangeShot, itor)) {
				delete pRangeShot;
				return false;
			}
			pOutAction->AddRangeShot(pRangeShot);
		}
		else if (_stricmp(itor->name(), "GRENADESHOT") == 0)
		{
			ZActorActionGrenadeShot* pGrenadeShot = new ZActorActionGrenadeShot;
			if (!ParseXml_ActionGrenadeShot(pGrenadeShot, itor)) {
				delete pGrenadeShot;
				return false;
			}
			pOutAction->AddGrenadeShot(pGrenadeShot);
		}
		else if (_stricmp(itor->name(), "SUMMON") == 0)
		{
			ZActorActionSummon* pSummon = new ZActorActionSummon;
			if (!ParseXml_ActionSummon(pSummon, itor)) {
				delete pSummon;
				return false;
			}
			pOutAction->AddSummon(pSummon);
		}
		else if (_stricmp(itor->name(), "SOUND") == 0)
		{
			ZActorActionSound* pSound = new ZActorActionSound;
			if (!ParseXml_ActionSound(pSound, itor))
			{
				delete pSound;
				return false;
			}
			pOutAction->AddSound(pSound);
		}
		else if (_stricmp(itor->name(), "EFFECT") == 0)
		{
			ZActorActionEffect* pEffect = new ZActorActionEffect;
			if (!ParseXml_ActionEffect(pEffect, itor))
			{
				delete pEffect;
				return false;
			}
			pOutAction->AddEffect(pEffect);
		}
	}
	return true;
}

bool ZActorActionManager::ParseXml_ActionShotCommonAttributes(ZActorActionShotCommon* pOutShot, rapidxml::xml_node<>* elem)
{
	string strSound;

	pOutShot->SetDelay(atoi(elem->first_attribute("delay")->value()) * 0.001f);
	pOutShot->SetDamage(atof(elem->first_attribute("damage")->value()));
	if (elem->first_attribute("pierce") != nullptr)
		pOutShot->SetPierce(atoi(elem->first_attribute("pierce")->value()) * 0.01f);
	else
		pOutShot->SetPierce(50 * 0.01f);
	if (elem->first_attribute("knockback") != nullptr)
		pOutShot->SetKnockbackForce(atoi(elem->first_attribute("knockback")->value()));
	else
		pOutShot->SetKnockbackForce(0);
	if (elem->first_attribute("sound") != nullptr)
		pOutShot->SetSound(elem->first_attribute("sound")->value());
	else
		pOutShot->SetSound("");

	return true;
}

bool ZActorActionManager::ParseXml_ActionMeleeShot(ZActorActionMeleeShot* pOutMeleeShot, rapidxml::xml_node<>* elem)
{
	if (!ParseXml_ActionShotCommonAttributes(pOutMeleeShot, elem))
		return false;


	pOutMeleeShot->SetRange(atof(elem->first_attribute("range")->value()));
	pOutMeleeShot->SetAngle(atof(elem->first_attribute("angle")->value()));
	if (elem->first_attribute("thrust") != nullptr)
	{
		if (_stricmp(elem->first_attribute("thrust")->value(), "true") == 0)
			pOutMeleeShot->SetThrust(true);
		else
			pOutMeleeShot->SetThrust(false);
	}
	else
		pOutMeleeShot->SetThrust(false);
	if (elem->first_attribute("uppercut") != nullptr)
	{
		if (_stricmp(elem->first_attribute("uppercut")->value(), "true") == 0)
			pOutMeleeShot->SetUpperCut(true);
		else
			pOutMeleeShot->SetUpperCut(false);
	}
	else
		pOutMeleeShot->SetUpperCut(false);

	return true;
}

bool ZActorActionManager::ParseXml_ActionRangeShot(ZActorActionRangeShot* pOutRangeShot, rapidxml::xml_node<>* elem)
{
	if (!ParseXml_ActionShotCommonAttributes(pOutRangeShot, elem))
		return false;

	rvector vDirMod;
	_RMeshPartsPosInfoType posPartsType = eq_parts_pos_info_end;

	if (elem->first_attribute("mesh") != nullptr)
		pOutRangeShot->SetMeshName(elem->first_attribute("mesh")->value());
	else
		pOutRangeShot->SetMeshName("");

	pOutRangeShot->SetSpeed(atof(elem->first_attribute("speed")->value()));
	pOutRangeShot->SetCollRadius(atof(elem->first_attribute("collradius")->value()));
	if (elem->first_attribute("dirmod") != nullptr)
	{
		sscanf(elem->first_attribute("dirmod")->value(), "%f %f %f", &vDirMod.x, &vDirMod.y, &vDirMod.z);
		pOutRangeShot->SetDirectionMod(vDirMod);
	}
	else
		pOutRangeShot->SetDirectionMod(rvector(0, 0, 0));

	if (elem->first_attribute("posparts") != nullptr)
	{
		if (_stricmp(elem->first_attribute("posparts")->value(), "head") == 0) posPartsType = eq_parts_pos_info_Head;
		if (_stricmp(elem->first_attribute("posparts")->value(), "lhand") == 0) posPartsType = eq_parts_pos_info_LHand;
		if (_stricmp(elem->first_attribute("posparts")->value(), "rhand") == 0) posPartsType = eq_parts_pos_info_RHand;
	}
	pOutRangeShot->SetPosPartsType(posPartsType);
	if (elem->first_attribute("zaxis") != nullptr)
		pOutRangeShot->SetZAxis(atoi(elem->first_attribute("zaxis")->value()));
	else
		pOutRangeShot->SetZAxis(0);

	if (elem->first_attribute("yaxis") != nullptr)
		pOutRangeShot->SetYAxis(atoi(elem->first_attribute("yaxis")->value()));
	else
		pOutRangeShot->SetYAxis(0);

	if (elem->first_attribute("dirTarget") != nullptr) {
		if (_stricmp(elem->first_attribute("dirTarget")->value(), "true") == 0)
			pOutRangeShot->SetDirTarget(true);
		else
			pOutRangeShot->SetDirTarget(false);
	}
	else
		pOutRangeShot->SetDirTarget(false);
	return true;
}

bool ZActorActionManager::ParseXml_ActionGrenadeShot(ZActorActionGrenadeShot* pOutGrenadeShot, rapidxml::xml_node<>* elem)
{
	_RMeshPartsPosInfoType partsType = eq_parts_pos_info_end;
	rvector posMod, dirMod;

	if (!ParseXml_ActionShotCommonAttributes(pOutGrenadeShot, elem))
		return false;

	pOutGrenadeShot->SetGrenadeType(atoi(elem->first_attribute("grenadetype")->value()));
	pOutGrenadeShot->SetItemID(atoi(elem->first_attribute("itemid")->value()));
	pOutGrenadeShot->SetZAxis(atoi(elem->first_attribute("zaxis")->value()));
	pOutGrenadeShot->SetYAxis(atoi(elem->first_attribute("yaxis")->value()));
	pOutGrenadeShot->SetForce(atoi(elem->first_attribute("force")->value()));
	if (elem->first_attribute("posparts") != nullptr)
	{
		if (_stricmp(elem->first_attribute("posparts")->value(), "head") == 0) partsType = eq_parts_pos_info_Head;
		if (_stricmp(elem->first_attribute("posparts")->value(), "lhand") == 0) partsType = eq_parts_pos_info_LHand;
		if (_stricmp(elem->first_attribute("posparts")->value(), "rhand") == 0) partsType = eq_parts_pos_info_RHand;
	}
	pOutGrenadeShot->SetPosPartsType(partsType);
	if (elem->first_attribute("posmod") != nullptr)
	{
		sscanf(elem->first_attribute("posmod")->value(), "%f %f %f", &posMod.x, &posMod.y, &posMod.z);
		pOutGrenadeShot->SetPosMod(posMod);
	}
	if (elem->first_attribute("dirmod") != nullptr)
	{
		sscanf(elem->first_attribute("dirmod")->value(), "%f %f %f", &dirMod.x, &dirMod.y, &dirMod.z);
		pOutGrenadeShot->SetPosMod(posMod);
	}

	return true;
}

bool ZActorActionManager::ParseXml_ActionSummon(ZActorActionSummon* pOutSummon, rapidxml::xml_node<>* elem)
{
	string name,item;
	pOutSummon->SetName(elem->first_attribute("name")->value());
	pOutSummon->SetDelay(atoi(elem->first_attribute("delay")->value()) * 0.001f);
	pOutSummon->SetRange(atoi(elem->first_attribute("range")->value()));
	pOutSummon->SetAngle(atoi(elem->first_attribute("angle")->value()));
	if (elem->first_attribute("route") != nullptr)
		pOutSummon->SetRoute(atoi(elem->first_attribute("route")->value()));
	else
		pOutSummon->SetRoute(0);
	if (elem->first_attribute("adjustplayernum") != nullptr)
	{
		if (_stricmp(elem->first_attribute("adjustplayernum")->value(), "true") == 0)
			pOutSummon->SetAdjustPlayerNum(true);
		else
			pOutSummon->SetAdjustPlayerNum(false);
	}
	else
		pOutSummon->SetAdjustPlayerNum(false);

	if (elem->first_attribute("drop") != nullptr)
	{
		pOutSummon->SetItemDrop(elem->first_attribute("drop")->value());
	}
	else
		pOutSummon->SetItemDrop("");

	return true;
}

bool ZActorActionManager::ParseXml_ActionSound(ZActorActionSound* pSound, rapidxml::xml_node<>* elem)
{

	pSound->SetDelay(atoi(elem->first_attribute("delay")->value()) * 0.001f);
	pSound->SetName(elem->first_attribute("sound")->value());
	return true;
}

bool ZActorActionManager::ParseXml_ActionEffect(ZActorActionEffect* pOutEffect, rapidxml::xml_node<>* elem)
{
	_RMeshPartsPosInfoType partsType = eq_parts_pos_info_end;
	rvector posMod, dirMod;

	pOutEffect->SetDelay(atoi(elem->first_attribute("delay")->value()) * 0.001f);
	pOutEffect->SetMeshName(elem->first_attribute("mesh")->value());

	if (elem->first_attribute("posparts") != nullptr)
	{
		if (elem->first_attribute("posparts")->value() == nullptr)
			partsType = eq_parts_pos_info_end;
		else
		{
			char* posParts = elem->first_attribute("posparts")->value();
			if (_stricmp(posParts, "head") == 0) partsType = eq_parts_pos_info_Head;
			if (_stricmp(posParts, "lhand") == 0) partsType = eq_parts_pos_info_LHand;
			if (_stricmp(posParts, "rhand") == 0) partsType = eq_parts_pos_info_RHand;
		}

	}
	pOutEffect->SetPosPartsType(partsType);


	if (elem->first_attribute("posmod") != nullptr)
	{
		if (elem->first_attribute("posmod")->value() != nullptr)
		{
			sscanf(elem->first_attribute("posmod")->value(), "%f %f %f", &posMod.x, &posMod.y, &posMod.z);
			pOutEffect->SetPosMod(posMod);
		}
		else
			pOutEffect->SetPosMod(rvector(1,1,1));
	}
	if (elem->first_attribute("dirmod") != nullptr)
	{
		if (elem->first_attribute("dirmod")->value() != nullptr)
		{
			sscanf(elem->first_attribute("dirmod")->value(), "%f %f %f", &dirMod.x, &dirMod.y, &dirMod.z);
			pOutEffect->SetDirMod(dirMod);
		}
		else
			pOutEffect->SetDirMod(rvector(1, 1, 1));
	}

	if (elem->first_attribute("zaxis") != nullptr)
		pOutEffect->SetZAxis(atoi(elem->first_attribute("zaxis")->value()));
	else
		pOutEffect->SetZAxis(0);

	pOutEffect->SetScale(atof(elem->first_attribute("scale")->value()));

	return true;
}

bool ZActorActionManager::AddAction(ZActorAction* pAction)
{
	// 중복 이름이면 실패
	ItorAction it = m_mapAction.find(pAction->GetName());
	if (it != m_mapAction.end())
		return false;

	m_mapAction[pAction->GetName()] = pAction;
	return true;
}

void ZActorActionManager::Clear()
{
	for (ItorAction it=m_mapAction.begin(); it!=m_mapAction.end(); ++it)
		delete it->second;
	m_mapAction.clear();
}

ZActorAction* ZActorActionManager::GetAction( const char* szName )
{
	ItorAction it = m_mapAction.find(szName);
	if (it != m_mapAction.end())
		return it->second;
	
	return NULL;
}

int ZActorActionManager::GetIndexOfAction(IActorAction* pAction)
{
	int idx=0;
	for (ItorAction it=m_mapAction.begin(); it!=m_mapAction.end(); ++it)
	{
		if (it->second == pAction)
			return idx;
		++idx;
	}
	return -1;
}

ZActorAction* ZActorActionManager::GetActionByIndex(int idx)
{
	int numAction = (int)m_mapAction.size();
	if (idx < 0 || numAction <= idx)
		return NULL;

	ItorAction it = m_mapAction.begin();
	for (int i=0; i<idx; ++i)
		++it;

	return it->second;
}