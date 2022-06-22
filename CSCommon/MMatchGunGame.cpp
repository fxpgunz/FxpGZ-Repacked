#include "stdafx.h"
#include "MMatchGunGame.h"
#include "MXml.h"
#include <vector>
#include "MMatchServer.h"

MGunGameData g_GunGameData;
int g_nGunGameMaxLevel = 0;
bool g_bRandomize = false;

bool InitializeGunGame()
{
	MXmlDocument	xmlDocument;

	xmlDocument.Create();

	if (!xmlDocument.LoadFromFile("gungame.xml"))
	{
		xmlDocument.Destroy();
		return false;
	}

	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlDocument.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "RULE"))
		{
			int nMaxLevel = 0;
			
			chrElement.GetAttribute(&nMaxLevel, "maxlevel");
			chrElement.GetAttribute(&g_bRandomize, "randomize");

			g_nGunGameMaxLevel = nMaxLevel;
		}

		if (!strcmp(szTagName, "WEAPON"))
		{
			int nDescID = 0;
			int nLevel = 0;
			int parts = 3;
			char szSlot[512];
			chrElement.GetAttribute(&nDescID, "id");
			chrElement.GetAttribute(&nLevel, "level");

			chrElement.GetAttribute(szSlot, "parts");

			if (!stricmp(szSlot, "melee"))
			{
				parts = 0;
			}
			else if (!stricmp(szSlot, "primary"))
			{
				parts = 1;
			}
			else if (!stricmp(szSlot, "secondary"))
			{
				parts = 2;
			}

			if (parts >= 3 || nLevel == 0)
			{
				continue;
			}

			GunGameData ggData;
			ggData.nItemID = nDescID;
			ggData.nItemParts = parts;
			ggData.nLevel = nLevel;

			g_GunGameData.push_back(ggData);

		}
	}

	xmlDocument.Destroy();

	return true;
}

int* GetGunGameItemSet(MGunGameData data, int nLevel)
{
	int* nItemSet = new int[3];
	memset(nItemSet, 0, sizeof(nItemSet));

	for (int i = 0; i < data.size(); ++i)
	{
		GunGameData gg = data[i];

		if (gg.nLevel == nLevel)
		{
			nItemSet[gg.nItemParts] = gg.nItemID;
		}
	}

	return nItemSet;
}

int GetGunGameMaxLevel()
{
	return g_nGunGameMaxLevel;
}

bool GetGunGameRuleRandomize()
{
	return g_bRandomize;
}

vector<GunGameData> GetGunGameData()
{
	return g_GunGameData;
}