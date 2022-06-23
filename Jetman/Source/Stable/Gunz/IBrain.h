#pragma once

#include "ZActorAnimation.h"
#include "ZTaskID.h"

class IBrain
{
public:
	virtual ~IBrain() {}

	virtual void Init( ZActor* pBody) = 0;
	virtual void Think( float fDelta) = 0;
	virtual void OnDamaged() = 0;

	virtual void OnBody_AnimEnter( ZA_ANIM_STATE nAnimState) = 0;
	virtual void OnBody_AnimExit( ZA_ANIM_STATE nAnimState) = 0;
	virtual void OnBody_OnTaskFinished( ZTASK_ID nLastID) = 0;
};