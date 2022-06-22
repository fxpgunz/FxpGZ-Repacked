#ifndef _MMATCHRULE_NINJA_H
#define _MMATCHRULE_NINJA_H


#include "MMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleNinja : public MMatchRuleSoloDeath  {
public:
	MMatchRuleNinja(MMatchStage* pStage);
	virtual ~MMatchRuleNinja() { }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_NINJA; }
};


#endif