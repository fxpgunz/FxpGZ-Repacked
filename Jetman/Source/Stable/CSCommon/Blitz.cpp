#include "stdafx.h"
#include "Blitz.h"

Blitz::Blitz()
{

}

Blitz::~Blitz()
{

}

Blitz* Blitz::GetInstance()
{
	static Blitz instance;
	return &instance;
}

bool Blitz::ReadXml(const char* fileName, MZFileSystem* fileSystem)
{
	rapidxml::xml_document<> doc;


	MZFile mzf;
	if (fileSystem)
	{
		if (!mzf.Open(fileName, fileSystem))
		{
			if (!mzf.Open(fileName))
				return false;
		}
	}
	else
	{
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

		if (_stricmp(itor->name(), "HONOR_LIST") == 0)
			ParseXml_HonorTag(itor);

		if (_stricmp(itor->name(), "CLASS_BOOK") == 0)
			ParseXml_ClassTag(itor);

		if (_stricmp(itor->name(), "SPAWN_LIST") == 0)
			ParseXml_SpawnTag(itor);

		if (_stricmp(itor->name(), "ROUTE_LIST") == 0)
			ParseXml_RouteTag(itor);

		if (_stricmp(itor->name(), "WEAPON") == 0)
			ParseXml_WeaponTag(itor);

		if (_stricmp(itor->name(), "EVENT") == 0)
			ParseXml_EventTag(itor);

		if (_stricmp(itor->name(), "UPGRADE") == 0)
			ParseXml_UpgradeTag(itor);

		if (_stricmp(itor->name(), "HELP_MESSAGE") == 0)
			ParseXml_HelpTag(itor);
	}

	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;
}

void Blitz::ParseXml_EventTag(rapidxml::xml_node<>* elem)
{
	rapidxml::xml_node<>* childNode = elem->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), "HONOR") == 0)
		{
			PlayerHonor node = { 0 };
			node.startHonor = atoi(itor->first_attribute("startHonor")->value());
			node.honorIncreaseTime = atoi(itor->first_attribute("autoIncHonorSec")->value());
			node.honorIncreaseValue = atoi(itor->first_attribute("autoIncHonor")->value());
			node.firstKillHonor = atoi(itor->first_attribute("firstKillHonor")->value());
			if (!ParseXml_PlayerHonorInfo(itor->first_node(), node)) {
				mlog("Error parsing PlayerHonorInfo\n");
				return;
			}
			m_playerHonorInfo = node;
		}
		if (_stricmp(itor->name(), "ENHANCE_PLAYER") == 0)
		{
			m_playerEnhancement.hpAp = atoi(itor->first_attribute("apHp")->value());
			m_playerEnhancement.dps = atoi(itor->first_attribute("dps")->value());
		}
		if (_stricmp(itor->name(), "ENHANCE_NPC") == 0)
		{
			m_NPCEnhancement.nextTime = atoi(itor->first_attribute("nextTime")->value());
			m_NPCEnhancement.maxCount = atoi(itor->first_attribute("maxCount")->value());
			m_NPCEnhancement.enhancedRatio = strtof(itor->first_attribute("enhancedRatio")->value(),nullptr);
		}
		if (_stricmp(itor->name(), "HONOR_ITEM_LIST") == 0)
		{
			HonorItemList node = {};
			node.spawnTime = atoi(itor->first_attribute("respawnTime")->value());
			if (!ParseXml_HonorItemList(itor->first_node(), node)) {
				mlog("Error parsing HonorItemList\n");
				return;
			}
			m_honorInfo = node;
		}
		if (_stricmp(itor->name(), "BUILDING") == 0)
		{
			Building node = { 0 };
			node.damageReduction = strtof(itor->first_attribute("reduceDamageRatioFromPlayer")->value(),nullptr);
			node.timeCheck = atoi(itor->first_attribute("checkDelay")->value());

			m_buildingEvents = node;
		}
	}
}

void Blitz::ParseXml_SpawnTag(rapidxml::xml_node<>* elem)
{
	rapidxml::xml_node<>* childNode = elem->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (_stricmp(itor->name(), "SPAWN") == 0)
		{
			SpawnList Node;

			if (!ParseXml_SpawnList(itor, Node)) {
				mlog("Error parsing spawnlist\n");
				return;
			}
			m_spawnList.push_back(Node);
		}
	}
}

bool Blitz::ParseXml_SpawnList(rapidxml::xml_node<>* elem, SpawnList& spawnList)
{
	strcpy_s(spawnList.actorName, elem->first_attribute("actor")->value());
	spawnList.npcCount = atoi(elem->first_attribute("num")->value());
	spawnList.teamID  = (MMatchTeam)atoi(elem->first_attribute("team")->value());

	return true;
}

void Blitz::ParseXml_RouteTag(rapidxml::xml_node<>* elem)
{
	rapidxml::xml_node<>* childNode = elem->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (_stricmp(itor->name(), "ROUTE") == 0)
		{
			RouteList Node;

			if (!ParseXml_RouteList(itor, Node)) {
				mlog("error parsing routeList\n");
				return;
			}
			m_routeList.push_back(Node);
		}
	}
}

bool Blitz::ParseXml_RouteList(rapidxml::xml_node<>* elem, RouteList& routeList)
{
	routeList.routeID = atoi(elem->first_attribute("id")->value());
	strcpy_s(routeList.route1, elem->first_attribute("route1")->value());
	strcpy_s(routeList.route2, elem->first_attribute("route2")->value());
	strcpy_s(routeList.route3, elem->first_attribute("route3")->value());
	strcpy_s(routeList.route4, elem->first_attribute("route4")->value());
	strcpy_s(routeList.route5, elem->first_attribute("route5")->value());
	strcpy_s(routeList.route6, elem->first_attribute("route6")->value());
	if (elem->first_attribute("route7") != nullptr)
		strcpy_s(routeList.route7, elem->first_attribute("route7")->value());
	else
		strcpy_s(routeList.route7, "");

	return true;
}

bool Blitz::ParseXml_HonorItemList(rapidxml::xml_node<>* elem, HonorItemList& honorList)
{
	for (auto itor = elem; itor; itor = itor->next_sibling())
		honorList.npcInfo.push_back(itor->first_attribute("actor")->value());

	return true;
}

void Blitz::ParseXml_WeaponTag(rapidxml::xml_node<>* elem)
{
	rapidxml::xml_node<>* childNode = elem->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), "DPS") == 0) {
			WeaponDPS node = { 0 };
			if (!ParseXml_WeaponDPS(itor, node)) {
				mlog("Error parsing dps\n");
				return;
			}
			m_weaponDPS = node;
		}
		if (_stricmp(itor->name(), "DELAY") == 0) {
			WeaponDelay node = { 0 };
			if (!ParseXml_WeaponDelay(itor, node)) {
				mlog("Error parsing weapon delay\n");
				return;
			}
			m_weaponDelay = node;
		}
	}
}



bool Blitz::ParseXml_WeaponDPS(rapidxml::xml_node<>* elem, WeaponDPS& weaponDps)
{
	weaponDps.smg = strtof(elem->first_attribute("smg")->value(),nullptr);
	weaponDps.rifle = strtof(elem->first_attribute("rifle")->value(),nullptr);
	weaponDps.machineGun = strtof(elem->first_attribute("machineGun")->value(),nullptr);
	weaponDps.pistol = strtof(elem->first_attribute("pistol")->value(),nullptr);
	weaponDps.revolver = strtof(elem->first_attribute("revolver")->value(),nullptr);
	weaponDps.shotGun = strtof(elem->first_attribute("shotGun")->value(),nullptr);
	weaponDps.roket = strtof(elem->first_attribute("roket")->value(),nullptr);
	weaponDps.dagger = strtof(elem->first_attribute("dagger")->value(),nullptr);
	weaponDps.katana = strtof(elem->first_attribute("katana")->value(),nullptr);
	weaponDps.doubleKatana = strtof(elem->first_attribute("doubleKatana")->value(), nullptr);

	return true;
}

bool Blitz::ParseXml_WeaponDelay(rapidxml::xml_node<>* elem, WeaponDelay& weaponDelay)
{
	weaponDelay.smg = strtof(elem->first_attribute("smg")->value(),nullptr);
	weaponDelay.rifle = strtof(elem->first_attribute("rifle")->value(),nullptr);
	weaponDelay.machineGun = strtof(elem->first_attribute("machineGun")->value(),nullptr);
	weaponDelay.pistol = strtof(elem->first_attribute("pistol")->value(),nullptr);
	weaponDelay.revolver = strtof(elem->first_attribute("revolver")->value(),nullptr);
	weaponDelay.shotGun = strtof(elem->first_attribute("shotGun")->value(),nullptr);
	weaponDelay.roket = strtof(elem->first_attribute("roket")->value(),nullptr);
	weaponDelay.dagger = strtof(elem->first_attribute("dagger")->value(),nullptr);
	weaponDelay.katana = strtof(elem->first_attribute("katana")->value(),nullptr);
	weaponDelay.doubleKatana = strtof(elem->first_attribute("doubleKatana")->value(),nullptr);
	return true;
}

bool Blitz::ParseXml_PlayerHonorInfo(rapidxml::xml_node<>* elem, PlayerHonor& playerHonor)
{

	for (auto itor = elem; itor; itor = itor->next_sibling())
		playerHonor.leaveHonorIncrement.push_back(atoi(itor->first_attribute()->value()));

	return true;
}

void Blitz::ParseXml_HonorTag(rapidxml::xml_node<>* elem)
{
	rapidxml::xml_node<>* childNode = elem->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;
		if (_stricmp(itor->name(), "PLYAER_HONOR") == 0)
		{
			PlayerKillReward node = {};
			if (!ParseXml_PlayerKillReward(itor, node)) {
				mlog("error parsing playerKill reward info\n");
				return;
			}
			m_playerKillReward = node;
		}
		if (_stricmp(itor->name(), "HONOR") == 0)
		{
			HonorReward node = {};
			if (!ParseXml_NpcRewardInfo(itor, node)) {
				mlog("error parsing honor reward info\n");
				return;
			}
			m_honorKillReward.push_back(node);
		}
		if (_stricmp(itor->name(), "HONOR_SOUND") == 0)
		{
			HonorRewardSound node = {};
			if (!ParseXml_HonorRewardSound(itor, node)) {
				mlog("Error parsing h onor sound info\n");
				return;
			}
			m_honorSound = node;
		}
	}
}

void Blitz::ParseXml_ClassTag(rapidxml::xml_node<>* elem)
{
	ClassBook node = {};
	if (!ParseXml_ClassBook(elem, node)) {
		mlog("error parsing classbook\n");
		return;
	}
	m_classBook = node;
}

void Blitz::ParseXml_UpgradeTag(rapidxml::xml_node<>* elem)
{

}

void Blitz::ParseXml_HelpTag(rapidxml::xml_node<>* elem)
{
	HelpMessage node = {};
	if (!ParseXml_Help(elem, node)) {
		mlog("Error parsing helpMesage Tag\n");
		return;
	}
	m_helpMessage = node;
}

bool Blitz::ParseXml_NpcRewardInfo(rapidxml::xml_node<>* elem, HonorReward& honorReward)
{
	honorReward.actorType = elem->first_attribute("actorType")->value();
	honorReward.singleKillReward = atoi(elem->first_attribute("single")->value());
	honorReward.allKillReward = atoi(elem->first_attribute("all")->value());

	return true;
}

bool Blitz::ParseXml_HonorRewardSound(rapidxml::xml_node<>* elem, HonorRewardSound& honorRewardSound)
{
	honorRewardSound.honorSound[0] = elem->first_attribute("lessGainHonor")->value();
	honorRewardSound.honorSound[1] = elem->first_attribute("regularGainHonor")->value();
	honorRewardSound.honorSound[2] = elem->first_attribute("moreGainHonor")->value();

	return true;
}

bool Blitz::ParseXml_PlayerKillReward(rapidxml::xml_node<>* elem, PlayerKillReward& playerKillReward)
{
	playerKillReward.singleKill = atoi(elem->first_attribute("single")->value());
	playerKillReward.assistReward = atoi(elem->first_attribute("assist")->value());
	playerKillReward.singleTotalDiv = strtof(elem->first_attribute("singleTotalDiv")->value(),nullptr);
	playerKillReward.assistTotalDiv = strtof(elem->first_attribute("assistTotalDiv")->value(),nullptr);
	playerKillReward.killDelay = strtof(elem->first_attribute("killDelay")->value(),nullptr);
	playerKillReward.assistDelay = strtof(elem->first_attribute("assistDelay")->value(),nullptr);

	return true;
}

bool Blitz::ParseXml_ClassBook(rapidxml::xml_node<>* elem, ClassBook& classBook)
{
	classBook.gladiator = atoi(elem->first_attribute("gladiator")->value());
	classBook.duelist = atoi(elem->first_attribute("duelist")->value());
	classBook.incinerator = atoi(elem->first_attribute("incinerator")->value());
	classBook.combatOfficer = atoi(elem->first_attribute("combatofficer")->value());
	classBook.assassin = atoi(elem->first_attribute("assassin")->value());
	classBook.terrorist = atoi(elem->first_attribute("terrorist")->value());

	return true;
}

bool Blitz::ParseXml_Help(rapidxml::xml_node<>* elem, HelpMessage& helpMsg)
{
	if (elem == nullptr)
		return false;

	if (elem->first_attribute("viewTime") == nullptr)
		return false;

	helpMsg.viewTime = atoi(elem->first_attribute("viewTime")->value());
	helpMsg.viewDist = atoi(elem->first_attribute("dist")->value());
	helpMsg.honor = atoi(elem->first_attribute("honor")->value());
	strcpy_s(helpMsg.sound, elem->first_attribute("sound")->value());

	return true;
}