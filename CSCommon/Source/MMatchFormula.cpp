#include "stdafx.h"
#include "MMatchFormula.h"
#include <math.h>
#include "MMatchItem.h"

#define MMTOC_FORMULA_TABLE						"FORMULA_TABLE"
#define MMTOC_LEVEL_MODIFIER					"LM"
#define MMTOC_NEED_EXP_LEVEL_MODIFIER			"NeedExpLM"
#define MMTOC_GETTING_EXP_LEVEL_MODIFIER		"GettingExpLM"
#define MMTOC_GETTING_BOUNTY_LEVEL_MODIFIER		"GettingBountyLM"


float MMatchFormula::m_fNeedExpLMTable[MAX_LEVEL+1];
float MMatchFormula::m_fGettingExpLMTable[MAX_LEVEL+1];
float MMatchFormula::m_fGettingBountyLMTable[MAX_LEVEL+1];

unsigned long int MMatchFormula::m_nNeedExp[MAX_LEVEL+1];
unsigned long int MMatchFormula::m_nGettingExp[MAX_LEVEL+1];
unsigned long int MMatchFormula::m_nGettingBounty[MAX_LEVEL+1];

bool MMatchFormula::Create()
{
	ZeroMemory(m_fNeedExpLMTable, sizeof(m_fNeedExpLMTable));
	ZeroMemory(m_fGettingExpLMTable, sizeof(m_fGettingExpLMTable));
	ZeroMemory(m_fGettingBountyLMTable, sizeof(m_fGettingBountyLMTable));

	ZeroMemory(m_nNeedExp, sizeof(m_nNeedExp));
	ZeroMemory(m_nGettingExp, sizeof(m_nGettingExp));
	ZeroMemory(m_nGettingBounty, sizeof(m_nGettingBounty));

	if (!ReadXml(FILENAME_MATCH_FORMULA))
	{
		return false;
	}

	PreCalcNeedExp();
	PreCalcGettingExp();
	PreCalcGettingBounty();

	return true;
}

bool MMatchFormula::ReadXml(const char* szXmlFileName)
{
	MXmlDocument	xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szXmlFileName))
	{
		xmlIniData.Destroy();
		return false;
	}

	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, MMTOC_FORMULA_TABLE))
		{
			char szTemp[64];
			chrElement.GetAttribute(szTemp, "id");
			if (!stricmp(szTemp, MMTOC_NEED_EXP_LEVEL_MODIFIER))
			{
				ParseNeedExpLM(chrElement);
			}
			else if (!stricmp(szTemp, MMTOC_GETTING_EXP_LEVEL_MODIFIER))
			{
				ParseGettingExpLM(chrElement);
			}
			else if (!stricmp(szTemp, MMTOC_GETTING_BOUNTY_LEVEL_MODIFIER))
			{
				ParseGettingBountyLM(chrElement);
			}
		}
	}

	xmlIniData.Destroy();
	return true;
}

void MMatchFormula::ParseNeedExpLM(MXmlElement& element)
{
	int iCount = element.GetChildNodeCount();
	MXmlElement childElement;
	char szTagName[64];

	for (int i = 0; i < iCount; i++)
	{
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, MMTOC_LEVEL_MODIFIER))
		{
			int nUpper = 0, nLower = 0;
			float fLM = 0.0f;

			childElement.GetAttribute(&nLower, "lower");	
			childElement.GetAttribute(&nUpper, "upper");
			childElement.GetContents(&fLM);

			for (int i = nLower; i <= nUpper; i++)
			{
				if (MMatchFormula::m_fNeedExpLMTable[i] <= 0.0001f) 
					MMatchFormula::m_fNeedExpLMTable[i] = fLM;
			}
		}
	}
}

void MMatchFormula::ParseGettingExpLM(MXmlElement& element)
{
	int iCount = element.GetChildNodeCount();
	MXmlElement childElement;
	char szTagName[64];

	for (int i = 0; i < iCount; i++)
	{
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, MMTOC_LEVEL_MODIFIER))
		{
			int nUpper = 0, nLower = 0;
			float fLM = 0.0f;

			childElement.GetAttribute(&nLower, "lower");	
			childElement.GetAttribute(&nUpper, "upper");
			childElement.GetContents(&fLM);

			for (int i = nLower; i <= nUpper; i++)
			{
				if (MMatchFormula::m_fGettingExpLMTable[i] <= 0.0001f) 
					MMatchFormula::m_fGettingExpLMTable[i] = fLM;
			}
		}
	}
}

void MMatchFormula::ParseGettingBountyLM(MXmlElement& element)
{
	int iCount = element.GetChildNodeCount();
	MXmlElement childElement;
	char szTagName[64];

	for (int i = 0; i < iCount; i++)
	{
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, MMTOC_LEVEL_MODIFIER))
		{
			int nUpper = 0, nLower = 0;
			float fLM = 0.0f;

			childElement.GetAttribute(&nLower, "lower");	
			childElement.GetAttribute(&nUpper, "upper");
			childElement.GetContents(&fLM);

			for (int i = nLower; i <= nUpper; i++)
			{
				if (MMatchFormula::m_fGettingBountyLMTable[i] <= 0.0001f) 
					MMatchFormula::m_fGettingBountyLMTable[i] = fLM;
			}
		}
	}
}


void MMatchFormula::PreCalcNeedExp()
{
	unsigned long int n;
	for (int lvl = 1; lvl <= MAX_LEVEL; lvl++)
	{
		n = (unsigned long int)((lvl * lvl * m_fNeedExpLMTable[lvl] * 100) + 0.5f);
		n = n * 2;	// 기획서보다 2배 더한다.
		m_nNeedExp[lvl] = m_nNeedExp[lvl - 1] + n;
	}
}

void MMatchFormula::PreCalcGettingExp()
{
	for (int lvl = 1; lvl <= MAX_LEVEL; lvl++)
	{
		unsigned long int nExp = 0;

		// È¹µæ°æÇèÄ¡ = LVL * LM * 20 + (LVL-1) * LM * 10
		m_nGettingExp[lvl] = ( (unsigned long int)((lvl * m_fGettingExpLMTable[lvl] * 20) +0.5f) + 
                            (unsigned long int)(((lvl-1) * m_fGettingExpLMTable[lvl] * 10) + 0.5f) );
	}
}

void MMatchFormula::PreCalcGettingBounty()
{
	for (int lvl = 1; lvl <= MAX_LEVEL; lvl++)
	{
		unsigned long int nExp = 0;

		// È¹µæ¹Ù¿îÆ¼ = TRUNC(LVL * LM * 1.2)
		m_nGettingBounty[lvl] = ( (unsigned long int)((lvl * m_fGettingBountyLMTable[lvl] * 1.2f) + 0.5f) );
	}
}


unsigned long int MMatchFormula::CalcPanaltyEXP(int nAttackerLevel, int nVictimLevel)
{
#define BOUNDARY_PANALTY_LEVEL		20

	// 20·¹º§ ÀÌÇÏ´Â °æÇèÄ¡°¡ ¶³¾îÁöÁö ¾Ê´Â´Ù.
	if (nVictimLevel <= BOUNDARY_PANALTY_LEVEL) return 0;

	// ÀÚ½Åº¸´Ù ·¹º§ÀÌ ³ôÀº »ç¶÷¿¡°Ô Á×À¸¸é ¼Õ½ÇÀÌ ¾ø´Ù.
	if (nAttackerLevel > nVictimLevel) return 0;

	// ÀÏÁ¤ ·¹º§Â÷ÀÌ¿¡´Â °æÇèÄ¡ ¼Õ½ÇÀÌ ¾ø´Ù
	if (abs(nAttackerLevel - nVictimLevel) <= (int(nVictimLevel/10) + 1) * 2) return 0;

	unsigned long int nExp = 0;
	unsigned long int nGettingExp = GetGettingExp(nAttackerLevel, nVictimLevel);
	int nGap = (nVictimLevel - nAttackerLevel);
	if (nGap < 0) nGap = 0;

	nExp = (unsigned long int)(nGettingExp * 0.5f * (1 + 0.1f * nGap));
	unsigned long int nMaxExp = m_nGettingExp[nVictimLevel] * 2;

	if (nExp > nMaxExp) nExp = nMaxExp;

	return (unsigned long int)nExp;
}

unsigned long int MMatchFormula::GetSuicidePanaltyEXP(int nLevel)
{
#define BOUNDARY_SUICIDE_PANALTY_LEVEL		5

	// 5·¹º§ ÀÌÇÏ´Â °æÇèÄ¡°¡ ¶³¾îÁöÁö ¾Ê´Â´Ù.
	if (nLevel <= BOUNDARY_SUICIDE_PANALTY_LEVEL) return 0;

	unsigned long int nExp = 0;
	unsigned long int nGettingExp = GetGettingExp(nLevel, nLevel);
	int nGap = 0;

	nExp = (unsigned long int)(nGettingExp * 0.5f * (1 + 0.1f * nGap));
	unsigned long int nMaxExp = m_nGettingExp[nLevel] * 2;

	if (nExp > nMaxExp) nExp = nMaxExp;

	// ÀÚ»ìÀÏ °æ¿ì °æÇèÄ¡ ¼Õ½ÇÀÌ µÎ¹è
	nExp = nExp * 2;

	return (unsigned long int)nExp;
}

int MMatchFormula::GetLevelFromExp(unsigned long int nExp)
{
	for (int level = 1; level < MAX_LEVEL; level++)
	{
		if(nExp < m_nNeedExp[level])
		{
			return level;
		}
	}

	return MAX_LEVEL;
}

unsigned long int MMatchFormula::GetGettingExp(int nAttackerLevel, int nVictimLevel)
{ 
	unsigned long int nExp, nAttackerMaxExp;
    
	nExp = m_nGettingExp[nVictimLevel];
	nAttackerMaxExp = m_nGettingExp[nAttackerLevel] * 2;
	if (nExp > nAttackerMaxExp) nExp = nAttackerMaxExp;

	return nExp;
}

unsigned long int MMatchFormula::GetGettingBounty(int nAttackerLevel, int nVictimLevel)
{
	unsigned long int nBounty, nAttackerMaxBounty;
    
	nBounty = m_nGettingBounty[nVictimLevel];
	nAttackerMaxBounty = m_nGettingBounty[nAttackerLevel] * 2;
	if (nBounty > nAttackerMaxBounty) nBounty = nAttackerMaxBounty;

	return nBounty;
}

int MMatchFormula::GetLevelPercent(unsigned long int nExp, int nNowLevel)
{
	unsigned long int nNowLevelExp, nNextLevelExp;
	int nPercent;

	nNowLevelExp = MMatchFormula::GetNeedExp(nNowLevel-1);
	nNextLevelExp = MMatchFormula::GetNeedExp(nNowLevel);

	nPercent = (int)(((float)(nExp - nNowLevelExp) / (float)(nNextLevelExp - nNowLevelExp)) * 100);
	if (nPercent < 0) nPercent = 0; 
	else if (nPercent > 100) nPercent = 100;

	return nPercent;
}

int MMatchFormula::GetClanBattlePoint(int nWinnerClanPoint, int nLoserClanPoint, int nOneTeamMemberCount)
{
	// http://iworks.maietgames.com/mdn/wiki.php/Å¬·£Àü ¿¡ °ø½ÄÀÌ ³ª¿ÍÀÖÀ½
/*
Delta ¸¸Å­ ÀÌ±ä Å¬·£¿¡ Á¡¼ö ´õÇÏ°í Áø Å¬·£¿¡ Á¡¼ö »©±â
(k=ÀÌ±ä Å¬·£ Á¡¼ö) (v=Áø Å¬·£ Á¡¼ö)

Delta1 = 5 / 1 + 10^((k-v)/1000)
Delta2 = tc / 10^((wc-lc)/50)
(tc=ÃÑÀÎ¿ø) (wc=ÀÌ±äÆÀÀÎ¿ø) (lc=ÁøÆÀÀÎ¿ø)
Delta = Delta1+Delta2
*/

	float Delta1 = 5.0f / (1 + pow(5.0f, float(nWinnerClanPoint-nLoserClanPoint) / 1000.0f));
	float Delta2 = float(nOneTeamMemberCount*2);
	int Delta = int(Delta1 + Delta2);
	if (Delta < 1) Delta = 1;

	return Delta;
}

// ¿ì¼± ¿ÞÂÊ °ÍÀ» °Ë»çÇÑ ÈÄ¿¡ ¿À¸¥ÂÊ °ÍÀ» °Ë»çÇØ¼­ ¹ÝÁö ÇÑ°³¸¸ Àû¿ëÇÑ´Ù. ÇÁ¸®¹Ì¾ö IPµµ °Ë»ç
float MMatchFormula::CalcXPBonusRatio(MMatchObject* pCharObj, MMatchItemBonusType nBonusType)
{
	float fBonusRatio = 0.0f;

	// ³Ý¸¶ºí PC¹æ º¸³Ê½º °è»ê ////////////////////////////////////////////////////////////
	if (pCharObj->GetAccountInfo()->m_nPGrade == MMPG_PREMIUM_IP)
	{
		const float PREMIUM_IP_BONUS = 0.2f;	// 1.2¹è

		fBonusRatio += PREMIUM_IP_BONUS;
	}

	// °æÇèÄ¡ ¾ÆÀÌÅÛ °è»ê //////////////////////////////////////////////////////////////////
	MMatchEquipedItem* pEquipedItems = &pCharObj->GetCharInfo()->m_EquipedItem;
	MMatchItem*		pItem;
	MMatchItemDesc* pItemDesc;
	float			fItemBounusRatio = 0.0f;

	switch (nBonusType)
	{
	case MIBT_SOLO:
		{
			for(int i=0; i<(int)MMCIP_END; ++i)
			{
				pItem= pEquipedItems->GetItem((MMatchCharItemParts)i);
				if(pItem)
					pItemDesc = pItem->GetDesc();
				else
					continue;
				fItemBounusRatio = pItemDesc->m_Bonus.m_fXP_SoloBonus;
				fBonusRatio = max(fBonusRatio, fItemBounusRatio);
			}
		}
		break;
	case MIBT_TEAM:
		{
			for(int i=0; i<(int)MMCIP_END; ++i)
			{
				pItem= pEquipedItems->GetItem((MMatchCharItemParts)i);
				if(pItem)
					pItemDesc = pItem->GetDesc();
				else
					continue;
				fItemBounusRatio = pItemDesc->m_Bonus.m_fXP_TeamBonus;
				fBonusRatio = max(fBonusRatio, fItemBounusRatio);
			}

		}
		break;
	case MIBT_QUEST:
		{
			for(int i=0; i<(int)MMCIP_END; ++i)
			{
				pItem= pEquipedItems->GetItem((MMatchCharItemParts)i);
				if(pItem)
					pItemDesc = pItem->GetDesc();
				else
					continue;
				fItemBounusRatio = pItemDesc->m_Bonus.m_fXP_QuestBonus;
				fBonusRatio = max(fBonusRatio, fItemBounusRatio);
			}
		}
		break;
	}

	MGetMatchServer()->CustomCheckEventObj( 2, pCharObj, (void*)(&fBonusRatio) );
	if (fBonusRatio > MAX_XP_BONUS_RATIO) fBonusRatio = MAX_XP_BONUS_RATIO;

	return fBonusRatio;
}


float MMatchFormula::CalcBPBounsRatio(MMatchObject* pCharObj, MMatchItemBonusType nBonusType )
{
	float fBonusRatio = 0.0f;

	// ³Ý¸¶ºí PC¹æ º¸³Ê½º °è»ê ////////////////////////////////////////////////////////////
	if (pCharObj->GetAccountInfo()->m_nPGrade == MMPG_PREMIUM_IP)
	{
		const float PREMIUM_IP_BONUS = 0.2f;	// 1.2¹è

		fBonusRatio += PREMIUM_IP_BONUS;
	}

	MMatchEquipedItem* pEquipedItems = &(pCharObj->GetCharInfo()->m_EquipedItem);
	MMatchItem*		pItem;
	MMatchItemDesc* pItemDesc;
	float			fItemBounusRatio = 0.0f;

	switch (nBonusType)
	{
	case MIBT_SOLO:
		{
			for(int i=0; i<(int)MMCIP_END; ++i)
			{
				pItem= pEquipedItems->GetItem((MMatchCharItemParts)i);
				if(pItem)
					pItemDesc = pItem->GetDesc();
				else
					continue;
				fItemBounusRatio = pItemDesc->m_Bonus.m_fBP_SoloBonus;
				fBonusRatio = max(fBonusRatio, fItemBounusRatio);
			}
		}
		break;
	case MIBT_TEAM:
		{
			for(int i=0; i<(int)MMCIP_END; ++i)
			{
				pItem= pEquipedItems->GetItem((MMatchCharItemParts)i);
				if(pItem)
					pItemDesc = pItem->GetDesc();
				else
					continue;
				fItemBounusRatio = pItemDesc->m_Bonus.m_fBP_TeamBonus;
				fBonusRatio = max(fBonusRatio, fItemBounusRatio);
			}
		}
		break;
	case MIBT_QUEST:
		{
			for(int i=0; i<(int)MMCIP_END; ++i)
			{
				pItem= pEquipedItems->GetItem((MMatchCharItemParts)i);
				if(pItem)
					pItemDesc = pItem->GetDesc();
				else
					continue;
				fItemBounusRatio = pItemDesc->m_Bonus.m_fBP_QuestBonus;
				fBonusRatio = max(fBonusRatio, fItemBounusRatio);
			}
		}
		break;
	}

	MGetMatchServer()->CustomCheckEventObj( 3, pCharObj, (void*)(&fBonusRatio) );
	if (fBonusRatio > MAX_BP_BONUS_RATIO) fBonusRatio = MAX_BP_BONUS_RATIO;

	return fBonusRatio;
}