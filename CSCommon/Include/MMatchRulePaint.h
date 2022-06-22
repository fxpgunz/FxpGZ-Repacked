#ifndef _MMATCHRULE_PAINT_H
#define _MMATCHRULE_PAINT_H


#include "MMatchRuleDeathMatch.h"

/**********************************************

Paintball Team and Paintball Solo

**********************************************/

class MMatchRuleSoloPaintball : public MMatchRuleSoloDeath  {
public:
	MMatchRuleSoloPaintball(MMatchStage* pStage);
	virtual ~MMatchRuleSoloPaintball() { }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_CLASSIC_SOLO; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleTeamPaintball : public MMatchRuleTeamDeath {
public:
	MMatchRuleTeamPaintball(MMatchStage* pStage);
	virtual ~MMatchRuleTeamPaintball()				{}
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_CLASSIC_TEAM; }
};

#endif