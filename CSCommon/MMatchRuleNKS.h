#ifndef _MMATCHRULE_NKS_H
#define _MMATCHRULE_NKS_H


#include "MMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleNKS : public MMatchRuleSoloDeath  {
public:
	MMatchRuleNKS(MMatchStage* pStage);
	virtual ~MMatchRuleNKS() { }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_NOKSTYLE; }
};


#endif