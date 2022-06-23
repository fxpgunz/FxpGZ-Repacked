#pragma once

struct mapCache
{
	int mapID;
	char mapName[48];
	char bspFile[48];
};

class ZMapCache
{
	vector<mapCache> m_normalMaps;
public:
	static ZMapCache* GetInstance();
	bool ReadXML(const char* fileName, MZFileSystem* fileSystem);
	bool ParseXML_Map(mapCache& cache, rapidxml::xml_node<>* elem);
	vector<mapCache>& GetMapCache() { return m_normalMaps; }

	bool Find(int id)
	{
		for (auto& item : m_normalMaps)
		{
			if (item.mapID == id)
				return true;
		}
		return false;
	}

	bool Find(const char* mapName)
	{
		for (auto& item : m_normalMaps)
		{
			if (StrStrI(mapName, item.mapName))
				return true;
		}
		return false;
	}

	void Clear()
	{
		m_normalMaps.clear();
	}
}; inline ZMapCache* ZGetMapCache() { return ZMapCache::GetInstance(); }