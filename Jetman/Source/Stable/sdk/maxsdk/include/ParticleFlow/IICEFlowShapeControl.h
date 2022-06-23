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

/// ICE Flow shape control interface ID
#define ICEFLOWSHAPE_CONTROL_INTERFACE Interface_ID(0x32af1746, 0x3cd775d4)

/*! Shape Control interface to change the shape of a given particle system.
    This is used to change the shape type of the shape action object from the particle flow simulation engine with the shape type coming from the nCache particle files generated from Softimage.
	This interface is implemented by 3ds Max. 
*/
class IICEFlowShapeControl : public FPStaticInterface
{
public:
	/*! \brief Sets the shape type into the specified object
	* \param [in] shapeLib the shape object whose shape will be changed
	* \param [in] shape the number of the shape as specified in the nCache file.
	* \return 0 if the shape was changed
	* \return 1 if the shape was not changed (the same)
	* \return 2 if the shape is not support and defaulted to point
	*/
	virtual int SetShape( INode * shapeLib, int shape ) = 0;

	/*! \brief Retrieves the single instance of the IICEFlowShapeControl
	*/
	static IICEFlowShapeControl* GetInstance()	{
		return static_cast<IICEFlowShapeControl*>(GetCOREInterface(ICEFLOWSHAPE_CONTROL_INTERFACE));
	}
};
