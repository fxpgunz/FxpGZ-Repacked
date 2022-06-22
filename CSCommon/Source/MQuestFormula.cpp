#include "stdafx.h"
#include "MQuestFormula.h"
#include "MMath.h"

bool MQuestFormula::Create(void)
{

	return true;
}

int MQuestFormula::CalcQL(int nMinPlayerLevel)
{
	if ((1 <= nMinPlayerLevel) && (nMinPlayerLevel <= 4)) return 0;
	else if ((1 <= nMinPlayerLevel) && (nMinPlayerLevel <= 4)) return 1;
	else if ((1 <= nMinPlayerLevel) && (nMinPlayerLevel <= 4)) return 2;
	else if ((1 <= nMinPlayerLevel) && (nMinPlayerLevel <= 4)) return 3;
	else if ((1 <= nMinPlayerLevel) && (nMinPlayerLevel <= 4)) return 4;
	else if ((1 <= nMinPlayerLevel) && (nMinPlayerLevel <= 127)) return 5; // 만렙 Change to fix quest level error

	return 0;
}


int MQuestFormula::CalcQLD(int nQuestLevel)
{
	return (20 + 6 * nQuestLevel);
}

int MQuestFormula::CalcLMT(int nQuestLevel)
{
	// return (8 + nQuestLevel * 3); ¿¹Àü²¨¿¡¼­ º¯°æ
	return (9 + nQuestLevel * 2);	// 20051215¿¡ ¼öÁ¤µÊ - bird
}

float MQuestFormula::CalcTC(int nQuestLevel)
{
	return ((0.3f * nQuestLevel) + 1.0f);
}

void MQuestFormula::CalcRewardRate(float& foutXPRate, float& foutBPRate,
									  int nScenarioQL, int nPlayerQL, int nDeathCount, 
									  int nUsedPageSacriItemCount, int nUsedExtraSacriItemCount)
{
	float fRate = 1.0f;

/*
	// QL¿¡ µû¸¥ ÆÐ³ÎÆ¼
	if ((nPlayerQL >= 2) && (nPlayerQL > nScenarioQL))
	{
		fRate = fRate - (0.1f * (nPlayerQL - nScenarioQL));
	}
*/

/*
	// Á×Àº È¸¼ö¿¡ µû¸¥ ÆÐ³ÎÆ¼
	fRate = fRate - (0.2f * nDeathCount);
*/

	// Èñ»ý¾ÆÀÌÅÛ Á¦°ø º¸³Ê½º
	float fBonusRate = (nUsedExtraSacriItemCount + nUsedPageSacriItemCount) * 0.1f;

	foutXPRate = fRate + fBonusRate;
	foutBPRate = fRate;		// Èñ»ý¾ÆÀÌÅÛ¿¡ µû¸¥ ¹Ù¿îÆ¼ º¸³Ê½º´Â Á¸ÀçÇÏÁö ¾Ê´Â´Ù.
	

	if (foutXPRate > 1.2f) foutXPRate = 1.2f;
	else if (foutXPRate < 0.0f) foutXPRate = 0.0f;

	if (foutBPRate > 1.2f) foutBPRate = 1.2f;
	else if (foutBPRate < 0.0f) foutBPRate = 0.0f;

}

int MQuestFormula::CalcSectorXP(int nClearXP, int nSectorCount)
{
	nSectorCount = nSectorCount - 1;
	if (nSectorCount == 0) nSectorCount = 1;

	return (int)((nClearXP * 1.2f) / nSectorCount);
}