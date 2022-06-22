#ifndef _ZRULE_CLASSIC_H
#define _ZRULE_CLASSIC_H


#include "ZRule.h"
#include "ZRuleDeathMatch.h"


class ZRuleSoloClassic : public ZRuleSoloDeathMatch
{
public:
	ZRuleSoloClassic(ZMatch* pMatch);
	virtual ~ZRuleSoloClassic();
};


class ZRuleTeamClassic : public ZRuleTeamDeathMatch
{
public:
	ZRuleTeamClassic(ZMatch* pMatch);
	virtual ~ZRuleTeamClassic();
};



#endif