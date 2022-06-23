//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../sfx.h"

/*! This class represents the interface for the Render Effects Manager.
    It enables the aggregation of several Effects where a single Effect* may be passed, most notably RendParams::effect.
    A pointer to this class may be obtained through a dynamic cast from an Effect*, most notably RendParams::effect.
*/
class IRenderEffectsMgr : public Effect
{
public:
	//! Returns number of Render Effects in manager's list.
	virtual int NumEffect() =0;

	//! Returns pointer to a specific Render Effect in manager's list.
	//! \param[in] index - The index of the Render Effect in the manager's list.
	//! \return The indexed Render Effect, NULL if invalid index
	virtual Effect *GetEffect(int index)=0;

	//! Replaces a Render Effect in the manager's list.
	//! \param[in] index - The index of the Render Effect in the manager's list.
	//! \param[in] effect - The Render Effect to place in the manager's list.
	virtual void SetEffect(int index, Effect *effect)=0;

	//! Adds a Render Effect to the manager's list.
	//! \param[in] effect - The Render Effect to place in the manager's list.
	virtual void AddEffect(Effect *effect)=0;

	//! Returns TRUE if any Render Effect in the manager's list is active
	virtual BOOL AnyActiveEffect()=0;

	//! Removes a Render Effect in the manager's list
	//! \param[in] index - The index of the Render Effect in the manager's list.
	virtual void DeleteEffect(int index)=0;

	//! Called by system to prepare the manager for rendering. Used internally.
	virtual void NotifyRendering()=0;

	//! Temporarily disables Render Effects. This only disables functionality implemented via Effect::Apply().
	//! Called by system when rendering multipass effects.
	virtual	void Disable()=0;

	//! Re-enable Render Effects disabled via Disable().
	virtual	void Enable()=0;
};


