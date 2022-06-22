#ifndef _ZRULE_NINJA_H
#define _ZRULE_NINJA_H


#include "ZRule.h"
#include "ZRuleDeathMatch.h"


class ZRuleNinja : public ZRuleSoloDeathMatch
{
public:
	ZRuleNinja(ZMatch* pMatch);
	virtual ~ZRuleNinja();
};



#endif