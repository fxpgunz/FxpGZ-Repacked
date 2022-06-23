#include "stdafx.h"
#include "ZMapCache.h"

ZMapCache* ZMapCache::GetInstance()
{
	static ZMapCache inst;
	return &inst;
}

bool ZMapCache::ReadXML(const char* fileName, MZFileSystem* fileSystem)
{
	MZFile mzf;
	rapidxml::xml_document<> doc;


	if (fileSystem)
	{
		if (!mzf.Open(fileName, fileSystem))
		{
			if (!mzf.Open(fileName))
			{
				return false;
			}
		}
	}

	char* buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	if (!mzf.Read(buffer, mzf.GetLength()))
	{
		delete[] buffer;
		return false;
	}

	if (!doc.parse<0>(buffer)) {
		mlog("error loading spymaplist\n");
		doc.clear();
		delete[] buffer;
		return false;
	}
	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), "MAP") == 0) {
			mapCache cache = { 0 };
			if (!ParseXML_Map(cache, itor)) {
				mlog("Error parsing mapcache tag");
				return false;
			}
			m_normalMaps.push_back(cache);
		}
	}

	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;
}

bool ZMapCache::ParseXML_Map(mapCache& cache, rapidxml::xml_node<>* elem)
{
	cache.mapID = atoi(elem->first_attribute("id")->value());
	strcpy_s(cache.mapName, elem->first_attribute("MapName")->value());
	strcpy_s(cache.bspFile, elem->first_attribute("BspFile")->value());

	return true;
}