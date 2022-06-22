#ifndef _ZRULE_NKS_H
#define _ZRULE_NKS_H


#include "ZRule.h"
#include "ZRuleDeathMatch.h"


class ZRuleNKS : public ZRuleSoloDeathMatch
{
public:
	ZRuleNKS(ZMatch* pMatch);
	virtual ~ZRuleNKS();
};



#endif