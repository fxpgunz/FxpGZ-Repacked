#include "stdafx.h"
#include "Demolition.h"

Demolition::Demolition()
{

}

Demolition::~Demolition()
{

}

void Demolition::Destroy()
{
	mapInfo.clear();
}

Demolition* Demolition::GetInstance()
{
	static Demolition Instance;

	return &Instance;
}

bool Demolition::ReadXML(const char* fileName, MZFileSystem* fileSystem)
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
	if (!mzf.Read(buffer, mzf.GetLength()))
	{
		mzf.Close();
		delete[] buffer;
		return false;
	}

	if (!doc.parse<0>(buffer))
		return false;
	rapidxml::xml_node<>* rootNode = doc.first_node();

	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (_stricmp(itor->name(), "MAP") == 0)
		{
			DemoMap map = { 0 };

			if (!ParseXML_Map(itor, map)) {
				mlog("Error parsing map information\n");
				return false;
			}
			mapInfo.push_back(map);
		}
	}
	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;
}

bool Demolition::ParseXML_Map(rapidxml::xml_node<>* node, DemoMap& map)
{
	if (node->first_attribute("id") == nullptr || node->first_attribute("redPos") == nullptr || node->first_attribute("bluePos") == nullptr)
		return false;

	map.mapIndex = atoi(node->first_attribute("id")->value());
	sscanf(node->first_attribute("redPos")->value(), "%f %f %f", &map.redPos.x, &map.redPos.y,&map.redPos.z);
	sscanf(node->first_attribute("bluePos")->value(), "%f %f %f", &map.bluePos.x, &map.bluePos.y, &map.bluePos.z);
	strcpy_s(map.mapName, node->first_attribute("name")->value());
	return true;
}