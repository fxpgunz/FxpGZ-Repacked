#include "stdafx.h"
#include "SkillMap.h"

SkillMap::SkillMap()
{

}

SkillMap::~SkillMap()
{

}

SkillMap* SkillMap::GetInstance()
{
	static SkillMap Instance;

	return &Instance;
}

bool SkillMap::ParseXML(const char* fileName, MZFileSystem* fileSystem)
{
	MZFile mzf;

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
	else
	{
		if (!mzf.Open(fileName))
		{
			return false;
		}
	}

	char* buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	if (!mzf.Read(buffer, mzf.GetLength()))
	{
		mzf.Close();
		delete[] buffer;
		return false;
	}

	rapidxml::xml_document<> doc;
	doc.parse<0>(buffer);
	rapidxml::xml_node<>* rootNode = doc.first_node();

	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;
		if (_stricmp(itor->name(), "MAP") == 0)
		{
			skillMapNode node = { 0 };
			if (!ParseXML_Map(itor, node))
			{
				mlog("Error parsing mapnode\n");
			}
			skillMapInfo.push_back(node);
		}
	}
	delete[] buffer;
	doc.clear();
	mzf.Close();
	return true;
}

bool SkillMap::ParseXML_Map(rapidxml::xml_node<>* element, skillMapNode& node)
{
	if (element->first_attribute("id") == nullptr || element->first_attribute("name") == nullptr)
		return false;

	node.id = static_cast<short>(atoi(element->first_attribute("id")->value()));
	strcpy_s(node.mapName, element->first_attribute("name")->value());

	return true;
}

void SkillMap::Clear()
{
	skillMapInfo.clear();
}