#ifndef _ZRULE_VAMPIRE_H
#define _ZRULE_VAMPIRE_H


#include "ZRule.h"
#include "ZRuleDeathMatch.h"


class ZRuleVampire : public ZRuleSoloDeathMatch
{
public:
	ZRuleVampire(ZMatch* pMatch);
	virtual ~ZRuleVampire();
};



#endif