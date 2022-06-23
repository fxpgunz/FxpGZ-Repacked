#pragma once

#include "MBaseGameType.h"

struct achievementNode
{
	short id;//Unique to each achievement;
	char type;///TODO: determine how to use this;
	char title[64];
	char desc[64];
	short requirement;
};

class Achievements
{
public:
	Achievements();
	virtual ~Achievements();
	static Achievements* GetInstance();

	void Destroy();

	bool readXml(const char* xmlName);
	bool checkRequirements(MMatchObject* targetPlayer, MMATCH_GAMETYPE type, int currVal, achievementNode& outValue);
	std::vector<achievementNode> Find(MMATCH_GAMETYPE achievementType);
private:
	bool parseXml_Achievement(MXmlElement& elem, achievementNode& node);
	std::vector<achievementNode> achievements;



}; inline Achievements* MGetAchievements() { return Achievements::GetInstance(); }