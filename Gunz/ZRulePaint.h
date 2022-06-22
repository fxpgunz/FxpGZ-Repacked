#ifndef _ZRULE_PAINT_H
#define _ZRULE_PAINT_H


#include "ZRule.h"
#include "ZRuleDeathMatch.h"


class ZRuleSoloPaintball : public ZRuleSoloDeathMatch
{
public:
	ZRuleSoloPaintball(ZMatch* pMatch);
	virtual ~ZRuleSoloPaintball();
};


class ZRuleTeamPaintball : public ZRuleTeamDeathMatch
{
public:
	ZRuleTeamPaintball(ZMatch* pMatch);
	virtual ~ZRuleTeamPaintball();
};



#endif