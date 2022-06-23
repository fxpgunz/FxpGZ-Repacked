#pragma once

struct Vec3Pos
{
	float x;
	float y;
	float z;
};

struct DemoMap
{
	int mapIndex;
	Vec3Pos redPos;
	Vec3Pos bluePos;
	char mapName[24];
};

class Demolition
{
public:
	vector<DemoMap> mapInfo;
public:
	Demolition();
	~Demolition();

	static Demolition* GetInstance();
	DemoMap GetMapIndex(int i) {
		if (i <= static_cast<int>(mapInfo.size()))
			return mapInfo.at(i);

		return {};
	}

	DemoMap GetMapIndex(const char* mapName) {

		for (int i = 0; i < static_cast<int>(mapInfo.size()); ++i)
		{
			if (_stricmp(mapName, mapInfo[i].mapName) == 0)
				return mapInfo[i];
		}
		return {};
	}


	void Destroy();

	bool ReadXML(const char* fileName, MZFileSystem* fileSystem = nullptr);
	bool ParseXML_Map(rapidxml::xml_node<>* node, DemoMap& map);

}; inline Demolition* MGetDemolition() { return Demolition::GetInstance(); }