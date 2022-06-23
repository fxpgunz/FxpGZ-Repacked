#include "stdafx.h"
#include "SpyMode.h"
SpyMode::SpyMode() {

}

SpyMode::~SpyMode() {

}


SpyMode* SpyMode::GetInstance()
{
	static SpyMode Instance;

	return &Instance;
}

bool SpyMode::ReadMapList(const char* fileName, MZFileSystem* fileSystem)
{
	m_SpyPlayerTable.clear();

	MZFile mzf;
	MXmlDocument doc;
	doc.Create();

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
		delete[] buffer;
		return false;
	}

	if (!doc.LoadFromMemory(buffer)) {
		mlog("error loading spymaplist\n");
		doc.Destroy();
		delete[] buffer;
		return false;
	}
	MXmlElement rootElement = doc.GetDocumentElement();
	for (int i = 0; i < rootElement.GetChildNodeCount(); ++i)
	{
		MXmlElement childElement = rootElement.GetChildNode(i);
		char tagName[32];
		childElement.GetTagName(tagName);

		if (stricmp("SPY_MAP", tagName) == 0)
		{
			SpyMap Node;
			if (!ParseXml_Map(childElement, Node))
			{
				mlog("error parsing spymap node\n");
				return false;
			}
			m_vSpyMapList.push_back(Node);
		}
	}

	delete[] buffer;
	mzf.Close();
	doc.Destroy();

	return true;
}

bool SpyMode::ParseXml_Map(MXmlElement& elem, SpyMap& node)
{
	elem.GetAttribute(&node.id, "id", 0);
	elem.GetAttribute(node.name, "name", "");
	elem.GetAttribute(&node.minPlayers, "minPlayers", 0);
	elem.GetAttribute(&node.maxPlayers, "maxPlayers", 0);
	elem.GetAttribute(&node.timeLimit, "limitTime", 0);
	elem.GetAttribute(&node.spyRevealTime, "spyOpenTime", 0);

	return true;
}

bool SpyMode::ReadMode(const char* fileName, MZFileSystem* fileSystem)
{
	MZFile mzf;
	MXmlDocument	xmlConfig;
	xmlConfig.Create();

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
		delete[] buffer;
		return false;
	}

	if (!xmlConfig.LoadFromMemory(buffer)) {
		delete[] buffer;
		xmlConfig.Destroy();
		return false;
	}

	MXmlElement rootElement = xmlConfig.GetDocumentElement();
	for (int i = 0; i < rootElement.GetChildNodeCount(); ++i)
	{
		MXmlElement childElement = rootElement.GetChildNode(i);
		char tagName[32];
		childElement.GetTagName(tagName);
		if (stricmp(tagName, "SELECT_SPY") == 0) {
			SpySelect Node = { 0 };
			if (!ParseXml_SpySelect(childElement, Node))
			{
				mlog("Error parsing spy select tag");
				return false;
			}
			m_SpySelect = Node;
		}
		if (stricmp(tagName, "SPY_ITEM_DESC") == 0)
		{
			SpyItems node;
			if (!ParseXml_SpyItems(childElement, node))
			{
				mlog("Error parsing spyitem element\n");
				return false;
			}
			m_vSpyItems.push_back(node);
		}
		if (stricmp(tagName, "SPY_TABLE") == 0)
		{
			SpyPlayerTable node = { 0 };
			if (!ParseXml_SpyTable(childElement, node))
			{
				mlog("error parsing spytable element\n");
				return false;
			}
			m_SpyPlayerTable.emplace(node.totalPlayers, node);
		}
		if (stricmp(tagName, "TRACER_TABLE") == 0)
		{
			TracerPlayerTable node = { 0 };
			if (!ParseXml_TracerTable(childElement, node))
			{
				mlog("error parsing tracer table element\n");
				return false;
			}
			m_TracerPlayerTable = node;
		}
	}

	delete[] buffer;
	mzf.Close();
	xmlConfig.Destroy();

	return true;
}

bool SpyMode::ParseXml_SpySelect(MXmlElement& elem, SpySelect& node)
{
	elem.GetAttribute(&node.spySelectTime, "selectSpyTime", 0);
	elem.GetAttribute(&node.defaultRating, "DefaultRating", 0);
	elem.GetAttribute(&node.selectedRating, "SelectedRating", 0);
	elem.GetAttribute(&node.minimumRating, "MinimumRating", 0);
	elem.GetAttribute(&node.maximumRating, "MaximumRating", 0);
	elem.GetAttribute(&node.roundFinishTime, "RounFinishWaitTime", 0);

	return true;
}

bool SpyMode::ParseXml_SpyItems(MXmlElement& elem, SpyItems& Node)
{
	int ID;
	elem.GetAttribute(&ID, "ID", 0);
	//elem.GetAttribute((int)&Node.ID, "ID", 0);
	elem.GetAttribute(Node.Desc, "Desc", "");
	Node.ID = ID;

	return true;
}

bool SpyMode::ParseXml_SpyTable(MXmlElement& elem, SpyPlayerTable& Node)
{
	elem.GetAttribute(&Node.totalPlayers, "TotalCount", 0);
	elem.GetAttribute(&Node.spyPlayers, "SpyCount", 0);
	elem.GetAttribute(&Node.hpap, "HPAP", 0);
	elem.GetAttribute(&Node.bagCount, "BAG", 0);
	elem.GetAttribute(&Node.lightCount, "LIGHT", 0);
	elem.GetAttribute(&Node.iceCount, "ICE", 0);
	elem.GetAttribute(&Node.smokeCount, "SMOKE", 0);

	return true;
}

bool SpyMode::ParseXml_TracerTable(MXmlElement& elem, TracerPlayerTable& node)
{
	elem.GetAttribute(&node.stunCount, "STUN", 0);
	elem.GetAttribute(&node.mineCount, "MINE", 0);
	return true;
}