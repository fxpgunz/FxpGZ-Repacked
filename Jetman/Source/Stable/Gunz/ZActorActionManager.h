#pragma once

class IActorAction;
class ZActorAction;
class ZActorActionShotCommon;
class ZActorActionMeleeShot;
class ZActorActionRangeShot;
class ZActorActionGrenadeShot;
class ZActorActionSummon;
class ZActorActionSound;
class ZActorActionEffect;

class ZActorActionManager
{
	typedef map<string, ZActorAction*>	MapAction;
	typedef MapAction::iterator			ItorAction;
	MapAction m_mapAction;

public:
	virtual ~ZActorActionManager();

	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName);
	bool ParseXmlFromStr(const char* szXmlContent);
	bool ParseXml_Action(ZActorAction* pOutAction, rapidxml::xml_node<>* elem);
	bool ParseXml_ActionShotCommonAttributes(ZActorActionShotCommon* pOutShot, rapidxml::xml_node<>* elem);
	bool ParseXml_ActionMeleeShot(ZActorActionMeleeShot* pOutMeleeShot, rapidxml::xml_node<>* elem);
	bool ParseXml_ActionRangeShot(ZActorActionRangeShot* pOutRangeShot, rapidxml::xml_node<>* elem);
	bool ParseXml_ActionGrenadeShot(ZActorActionGrenadeShot* pOutGrenadeShot, rapidxml::xml_node<>* elem);
	bool ParseXml_ActionSummon(ZActorActionSummon* pOutSummon, rapidxml::xml_node<>* elem);
	bool ParseXml_ActionSound(ZActorActionSound* pOutSound, rapidxml::xml_node<>* elem);
	bool ParseXml_ActionEffect(ZActorActionEffect* pOutEffect, rapidxml::xml_node<>* elem);
	bool AddAction(ZActorAction* pAction);
	int GetCount() { return (int)m_mapAction.size(); }
	void Clear();

	virtual ZActorAction* GetAction(const char* szName);

	int GetIndexOfAction(IActorAction* pAction);		// 매니저 내에서 액션의 번호를 얻는다
	ZActorAction* GetActionByIndex(int idx);			// 액션 번호로 액션을 찾는다
};