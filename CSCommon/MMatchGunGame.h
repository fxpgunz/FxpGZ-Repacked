#ifndef MMATCHGUNGAME_H
#define MMATCHGUNGAME_H

struct GunGameData
{
	int nItemID;
	int nLevel;
	int nItemParts;
};

typedef vector<GunGameData> MGunGameData;

bool InitializeGunGame();
int* GetGunGameItemSet(MGunGameData data, int nLevel);
int GetGunGameMaxLevel();
bool GetGunGameRuleRandomize();
vector<GunGameData> GetGunGameData();


#endif