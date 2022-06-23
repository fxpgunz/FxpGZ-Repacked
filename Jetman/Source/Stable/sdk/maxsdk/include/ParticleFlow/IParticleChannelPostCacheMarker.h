//**************************************************************************/
// Copyright (c) 1998-2013 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/

/*! \file IParticleChannelPostCacheMarker.h
    \brief A particle channel to identify a channel container
			that undergoes a post-cache processing by either
			Cache Disk or Cache Selective PFlow operators.
			It does not carry any particle-specific information,
			it's just a marker for PFlow operators to distinguish
			the particle processing to be done during post-cache stage.
*/

#pragma once

#include "IParticleChannelInt.h"

// standard particle channel "PostCacheMarker"
// interface IDs
#define PARTICLECHANNELPOSTCACHEMARKERR_INTERFACE Interface_ID(0xc928d443, 0x1eb34500)
#define PARTICLECHANNELPOSTCACHEMARKERW_INTERFACE Interface_ID(0xc928d443, 0x1eb34501)

#define GetParticleChannelPostCacheMarkerRInterface(obj) ((IParticleChannelIntR*)obj->GetInterface(PARTICLECHANNELPOSTCACHEMARKERR_INTERFACE))
#define GetParticleChannelPostCacheMarkerWInterface(obj) ((IParticleChannelIntW*)obj->GetInterface(PARTICLECHANNELPOSTCACHEMARKERW_INTERFACE))


