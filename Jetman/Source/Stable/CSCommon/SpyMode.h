#pragma once

#include <vector>
#include "MZFileSystem.h"
#include "MXml.h"
using namespace std;

struct SpyMap
{
	int id;
	char name[20];//larger than neccessary for adding new maps with possibly longer names
	int minPlayers;
	int maxPlayers;
	int timeLimit;
	int spyRevealTime;
};

//TODO: determine what the rating values are used for
struct SpySelect
{
	int spySelectTime;
	int defaultRating;
	int selectedRating;
	int minimumRating;
	int maximumRating;
	int roundFinishTime;
};

struct SpyItems
{
	unsigned int ID;
	char Desc[6];
};

//TODO: use shorts instead?
struct SpyPlayerTable
{
	int totalPlayers;
	int spyPlayers;
	int hpap;
	int bagCount;
	int lightCount;
	int iceCount;
	int smokeCount;
};

struct TracerPlayerTable
{
	int stunCount;
	int mineCount;
};

class SpyMode
{
private:
	bool ParseXml_Map(MXmlElement& elem, SpyMap& node);
	bool ParseXml_SpySelect(MXmlElement& elem, SpySelect& node);
	bool ParseXml_SpyItems(MXmlElement& elem, SpyItems& node);
	bool ParseXml_SpyTable(MXmlElement& elem, SpyPlayerTable& node);
	bool ParseXml_TracerTable(MXmlElement& elem, TracerPlayerTable& node);
	vector<SpyMap> m_vSpyMapList;
	vector<SpyItems> m_vSpyItems;
	map<int, SpyPlayerTable> m_SpyPlayerTable;
	SpySelect		m_SpySelect;
	TracerPlayerTable m_TracerPlayerTable;
public:
	SpyMode();
	~SpyMode();

	bool ReadMapList(const char* FileName, MZFileSystem* fileSystem = nullptr);
	bool ReadMode(const char* FileName, MZFileSystem* fileSystem = nullptr);
	static SpyMode* GetInstance();

	auto GetSpyMapList()
	{
		return m_vSpyMapList;
	}

	SpyMap GetSpyMap(int mapID)
	{
		for (auto Obj : m_vSpyMapList)
		{
			if (Obj.id == mapID)
				return Obj;
		}
		return{};
	}

	SpyMap GetSpyMap(const char* GetMapName)
	{
		for (auto Obj : m_vSpyMapList)
		{
			if (stricmp(GetMapName, Obj.name) == 0)
				return Obj;
		}
		return{};
	}

	auto GetSpyItems()
	{
		return m_vSpyItems;
	}

	auto GetSpySelect()
	{
		return m_SpySelect;
	}

	auto GetSpyTable()
	{
		return m_SpyPlayerTable;
	}

	//Use this to handle sending weapons in packet
	auto GetSpyTableElement(int totalPlayerCount)
	{
		return m_SpyPlayerTable.find(totalPlayerCount)->second;
	}

	auto GetTracerTable()
	{
		return m_TracerPlayerTable;
	}

}; inline SpyMode* MGetSpyMode() { return SpyMode::GetInstance(); }