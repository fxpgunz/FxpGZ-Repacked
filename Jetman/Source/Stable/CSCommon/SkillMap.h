#pragma once
#include <vector>
#include "MZFileSystem.h"

struct skillMapNode
{
	short id;
	char mapName[32];
};

class SkillMap
{
private:
	std::vector<skillMapNode> skillMapInfo;
public:
	SkillMap();
	~SkillMap();
	static SkillMap* GetInstance();
	bool ParseXML(const char* fileName, MZFileSystem* fileSystem = nullptr);
	bool ParseXML_Map(rapidxml::xml_node<>* element, skillMapNode& node);
	void Clear();
	const skillMapNode GetMapIndex(const char* mapName) const
	{
		for (auto& mapInfo : skillMapInfo)
		{
			if (_stricmp(mapInfo.mapName, mapName) == 0)
				return mapInfo;
		}
		return {};
	}

	bool FindMap(const char* mapName)
	{
		for (auto& mapInfo : skillMapInfo)
		{
			if (_stricmp(mapInfo.mapName, mapName) == 0)
				return true;
		}
		return false;
	}

	const vector<skillMapNode> GetSkillMaps() const { return skillMapInfo; }

}; inline SkillMap* MGetSkillMap() { return SkillMap::GetInstance(); }