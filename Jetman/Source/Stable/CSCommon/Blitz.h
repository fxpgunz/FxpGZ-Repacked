#pragma once

#include <vector>
#include "MZFileSystem.h"


struct ClassBook {
	int gladiator;
	int duelist;
	int incinerator;
	int combatOfficer;
	int assassin;
	int terrorist;
};

struct PlayerHonor {
	int startHonor;
	int honorIncreaseTime;
	int honorIncreaseValue;
	int firstKillHonor;
	vector<int> leaveHonorIncrement;//idk what this is for????
};

struct PlayerKillReward {
	int singleKill;
	int assistReward;
	///TODO: determine how to use these
	float singleTotalDiv;
	float assistTotalDiv;
	///TODO: determine what these were used for
	float killDelay;
	float assistDelay;
};

struct HonorReward {
	string actorType; 
	int singleKillReward = 0; //auto 0
	int allKillReward = 0;//auto 0 //idk what this is for????
};

struct HonorRewardSound {
	string honorSound[3];
};

struct WeaponDPS
{
	float smg;
	float rifle;
	float machineGun;
	float pistol;
	float revolver;
	float shotGun;
	float roket;
	float dagger;
	float katana;
	float doubleKatana;
};

struct WeaponDelay
{
	float smg;
	float rifle;
	float machineGun;
	float pistol;
	float revolver;
	float shotGun;
	float roket;
	float dagger;
	float katana;
	float doubleKatana;
};

struct PlayerEnhancement
{
	int hpAp;
	int dps;
};

//TODO: structs
struct SpawnList
{
	char actorName[24];
	int npcCount;
	MMatchTeam teamID;
};

struct RouteList
{
	int routeID;
	char route1[24];
	char route2[24];
	char route3[24];
	char route4[24];
	char route5[24];
	char route6[24];
	char route7[24];
};

struct HonorItemList
{
	int spawnTime;
	vector<string> npcInfo;
};

///TODO: read <UPGRADE/> Tag
struct Upgrade
{
	int requireHonor[4];
	int dps[4];
	float reduceShotDelayRatio[4];
	int hpAp[4];
	int enchantFire[4];
	float fireDamageDuration;
	float magazineRatio[4];
	float reviveRatio[4];
};

struct EnhanceNPC
{
	int nextTime;
	int maxCount;
	float enhancedRatio;
};

///TODO: read <BUILDING/> Tag
struct Building
{
	float damageReduction;
	int	  timeCheck;
	///TODO: fillthe rest of this
};

///TODO: read <REWARD/> Tag
struct Reward
{

};

///TODO: read <EVENT_MESSAGE/> Tag
struct EventMessage
{

};

///TODO: read <CLASS_TABLE/> Tag
struct ClassTable
{

};

///TODO: read <HELP_MESSAGE/> Tag
struct HelpMessage
{
	int viewTime;
	int viewDist;
	int honor;///TODO : determine what maiet used this for
	char sound[18];
};

class Blitz
{
private:
	//TODO:
	vector<SpawnList> m_spawnList;
	vector<RouteList> m_routeList;
	HonorItemList m_honorInfo;
	PlayerEnhancement m_playerEnhancement;
	WeaponDPS m_weaponDPS;
	WeaponDelay m_weaponDelay;
	PlayerHonor m_playerHonorInfo;
	vector<HonorReward> m_honorKillReward;
	PlayerKillReward m_playerKillReward;
	HonorRewardSound m_honorSound;
	ClassBook m_classBook;
	Building  m_buildingEvents;
	EnhanceNPC m_NPCEnhancement;
	EventMessage m_eventMessage;
	HelpMessage m_helpMessage;
	Upgrade		m_upgradeInfo;
public:
	Blitz();
	~Blitz();

	static Blitz* GetInstance();

	bool ReadXml(const char* fileName, MZFileSystem* fileSystem = nullptr);
	bool ParseXml_SpawnList(rapidxml::xml_node<>* elem, SpawnList& spawnList);
	bool ParseXml_RouteList(rapidxml::xml_node<>* elem, RouteList& routeList);
	bool ParseXml_HonorItemList(rapidxml::xml_node<>* elem, HonorItemList& honorList);
	bool ParseXml_WeaponDPS(rapidxml::xml_node<>* elem, WeaponDPS& weaponDPS);
	bool ParseXml_WeaponDelay(rapidxml::xml_node<>* elem, WeaponDelay& weaponDelay);
	bool ParseXml_PlayerHonorInfo(rapidxml::xml_node<>* elem, PlayerHonor& playerHonor);
	bool ParseXml_NpcRewardInfo(rapidxml::xml_node<>* elem, HonorReward& honorReward);
	bool ParseXml_HonorRewardSound(rapidxml::xml_node<>* elem, HonorRewardSound& honorRewardSound);
	bool ParseXml_PlayerKillReward(rapidxml::xml_node<>* elem, PlayerKillReward& playerKillReward);
	bool ParseXml_ClassBook(rapidxml::xml_node<>* elem, ClassBook& classBook);
	bool ParseXml_Help(rapidxml::xml_node<>* elem, HelpMessage& helpMsg);
	//new parse functions
	void ParseXml_HonorTag(rapidxml::xml_node<>* elem);
	void ParseXml_WeaponTag(rapidxml::xml_node<>* elem);
	void ParseXml_RouteTag(rapidxml::xml_node<>* elem);
	void ParseXml_SpawnTag(rapidxml::xml_node<>* elem);
	void ParseXml_ClassTag(rapidxml::xml_node<>* elem);
	void ParseXml_EventTag(rapidxml::xml_node<>* elem);
	void ParseXml_UpgradeTag(rapidxml::xml_node<>* elem);
	void ParseXml_HelpTag(rapidxml::xml_node<>* elem);

	const auto& GetPlayerEnhancement() {
		return m_playerEnhancement;
	}
	const auto& GetSpawnList() {
		return m_spawnList;
	}

	const auto& GetRouteList() {
		return m_routeList;
	}

	const auto& GetHonorNpcInfo() {
		return m_honorInfo;
	}

	const auto& GetPlayerHonorInfo() {
		return m_playerHonorInfo;
	}

	const auto& GetHonorRewardSound(int soundType) {
		return m_honorSound.honorSound[soundType];
	}

	const auto& GetPlayerRewardKill() {
		return m_playerKillReward;
	}
	///TODO: get the rest of the autos correct, I dont want local copy's, i want references
	const auto& GetHelpMessage() {
		return m_helpMessage;
	}

	const HonorReward GetHonorKillReward(const char* actorType) {
		auto itor = std::find_if(m_honorKillReward.begin(), m_honorKillReward.end(), [&](const HonorReward& item) {
			return(_stricmp(actorType, item.actorType.c_str()) == 0); });
		if (itor != m_honorKillReward.end())
		{
			return *itor;
		}
		HonorReward noReward;
		return noReward;
	}
	
	RouteList GetRouteListByID(int routeID) {
		for (auto itor = m_routeList.begin(); itor != m_routeList.end(); ++itor)
		{
			RouteList route = *itor;
			if (route.routeID == routeID)
				return route;
		}
		RouteList list;
		memset(&list, 0, sizeof(RouteList));
		return list;
	}

	const auto& GetWeaponDPS() {
		return m_weaponDPS;
	}

	const auto& GetWeaponDelay() {
		return m_weaponDelay;
	}

	const auto& GetClassBook() {
		return m_classBook;
	}

	const auto& GetBuildingEvents() {
		return m_buildingEvents;
	}

	const auto& GetNpcEnhanceInfo() {
		return m_NPCEnhancement;
	}

	const Upgrade* GetUpgrade(const int upgradeIndex, const int honorPoints, const int currHonorLevel)
	{
		if (honorPoints < m_upgradeInfo.requireHonor[currHonorLevel])
			return nullptr;

		//TODO: finish
		return nullptr;
	}

}; inline Blitz* MGetBlitz() { return Blitz::GetInstance(); }