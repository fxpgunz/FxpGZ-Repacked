//*****************************************************************************/
// Copyright 2010 Autodesk, Inc.  All rights reserved. 
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//*****************************************************************************/
#pragma once

#include "../ifnpub.h"
#include "../GetCOREInterface.h"

// --- Interface IDs
#define ICEFLOWPARTICLE_FACTORY_INTERFACE Interface_ID(0xf285a9a, 0x39587ad3)

/*! \brief Factory interface for the Particle Flow creation from a plugin.
    This is used to simplify the use of nCaches in 3ds Max when coming from Softimage XSI.
	This interface is implemented by 3ds Max. 
*/
class IICEFlowParticleFactory : public FPStaticInterface
{
public:
	/*! \brief Creates and adds to the scene a default ICE Flow particle structure.
	* \param [in] _name The name of the instance to create. This name is usually coming from the Manifest from the Delegate.
	* \param [in] _path The path of the nCache file that will be connected to this construct.
	* \note It is mandatory to have in the nCache files the following attributes : position, id, color, size, and mass
	* \return the created INode
	*/
	virtual INode * CreateFileBirth( const MCHAR * _name, const MCHAR * _path ) = 0;

	/*! \brief Retrieves the single instance of the ICEFlowParticleFactory
	*/
	static IICEFlowParticleFactory* GetInstance()	{
		return static_cast<IICEFlowParticleFactory*>(GetCOREInterface(ICEFLOWPARTICLE_FACTORY_INTERFACE));
	}
};

/*@}*/